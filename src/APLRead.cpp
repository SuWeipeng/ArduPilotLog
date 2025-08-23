#include <QDataStream>
#include <QRegularExpressionValidator>
#include <QFileInfo>
#include "APLRead.h"
#include "mainwindow.h"
#include "APLDataCache.h"
#include "LogStructure.h"

APL_LOGGING_CATEGORY(APLREAD_LOG,        "APLReadLog")

#define NAME_LEN    4
#define FORMAT_LEN 16
#define LABELS_LEN 64

APLRead* APLRead::_instance;

static LFMT FMT[256] = {};

// 优化1：添加格式字符长度预计算表 - 兼容旧版本C++编译器
static int FORMAT_CHAR_LENGTHS[256];

// 初始化查找表的函数
static void initFormatCharLengths() {
    static bool initialized = false;
    if (initialized) return;
    
    // 初始化所有值为0
    memset(FORMAT_CHAR_LENGTHS, 0, sizeof(FORMAT_CHAR_LENGTHS));
    
    // 设置已知格式字符的长度
    FORMAT_CHAR_LENGTHS['a'] = 64;
    FORMAT_CHAR_LENGTHS['b'] = 1;
    FORMAT_CHAR_LENGTHS['B'] = 1;
    FORMAT_CHAR_LENGTHS['M'] = 1;
    FORMAT_CHAR_LENGTHS['h'] = 2;
    FORMAT_CHAR_LENGTHS['H'] = 2;
    FORMAT_CHAR_LENGTHS['c'] = 2;
    FORMAT_CHAR_LENGTHS['C'] = 2;
    FORMAT_CHAR_LENGTHS['i'] = 4;
    FORMAT_CHAR_LENGTHS['I'] = 4;
    FORMAT_CHAR_LENGTHS['f'] = 4;
    FORMAT_CHAR_LENGTHS['e'] = 4;
    FORMAT_CHAR_LENGTHS['E'] = 4;
    FORMAT_CHAR_LENGTHS['L'] = 4;
    FORMAT_CHAR_LENGTHS['d'] = 8;
    FORMAT_CHAR_LENGTHS['q'] = 8;
    FORMAT_CHAR_LENGTHS['Q'] = 8;
    FORMAT_CHAR_LENGTHS['n'] = 4;
    FORMAT_CHAR_LENGTHS['N'] = 16;
    FORMAT_CHAR_LENGTHS['Z'] = 64;
    
    initialized = true;
}

// 预编译正则表达式 - 但保持原有的检查逻辑不变
static QRegularExpression NAME_REGEX("^[A-Z0-9]{1,4}$");
static QRegularExpression FORMAT_REGEX("^[A-Za-z]{1,16}$");  
static QRegularExpression LABELS_REGEX("^[A-Za-z0-9,]{1,64}$");

APLRead::APLRead()
{
    _instance = this;
    _resetDataBase();
    _worker       = new APLReadWorker();
    _workThread   = new QThread(this);
    _worker->moveToThread(_workThread);

    connect(this, &APLRead::startRunning, _worker, &APLReadWorker::decodeLogFile);
    connect(_workThread, &QThread::finished, _worker, &QObject::deleteLater);
    connect(_worker, &APLReadWorker::send_process, this, &APLRead::calc_process);
    connect(_worker, &APLReadWorker::fileOpened, this, &APLRead::getFileOpened);

    _workThread->start();
}

APLRead::~APLRead()
{
    qCDebug(APLREAD_LOG)<< "APLRead::~APLRead()";

    _workThread->quit();
    _workThread->wait();

    delete _workThread;
}

void APLRead::_resetDataBase()
{
    for(int i=0; i<256; i++)
        _resetFMT(i);
}

void  APLRead::_resetFMT(int i)
{
    FMT[i].id     = 0;
    FMT[i].name   = "";
    FMT[i].format = "";
    FMT[i].valid  = true;
}

void APLRead::getFileDir(const QString &file_dir)
{
    _file_name = QFileInfo(file_dir).fileName();
    _file_path = QFileInfo(file_dir).absolutePath();
    MainWindow::getMainWindow()->setWindowTitle(QString("ArduPilotLog ").append(file_dir));
    MainWindow::getMainWindow()->ui().progressBar->setValue(0);
    MainWindow::getMainWindow()->ui().progressBar->setVisible(0);
    if(MainWindow::getMainWindow()->db().isOpen()){
        MainWindow::getMainWindow()->db().close();
    }
    _resetDataBase();
    APLDataCache::get_singleton()->reset();
    getDatastream(file_dir);
}

void APLRead::getFileOpened()
{
    MainWindow::getMainWindow()->ui().progressBar->setVisible(0);
    emit fileOpened();
}

void APLRead::getDatastream(const QString &file_dir)
{
    if(!file_dir.isEmpty()){
        MainWindow::getMainWindow()->ui().progressBar->setVisible(1);
        emit startRunning(file_dir);
    }
}

void APLRead::calc_process(qint64 pos, qint64 size)
{
    int process = ((float)pos/size)*10000;
    MainWindow::getMainWindow()->ui().progressBar->setValue(process);
}

void APLReadWorker::_decode(const uchar* p_data, qint64 data_size)
{

    const uchar* ptr = p_data;
    const uchar* end = p_data + data_size;
    int progress_counter = 0;

    // 预计算头部字节组合，避免重复比较
    const uint16_t valid_header = (HEAD_BYTE1 << 8) | HEAD_BYTE2;


    while (ptr < end - LOG_PACKET_HEADER_LEN)
    {
        // 保持原有的进度更新逻辑
        if (++progress_counter > 102400) {
            emit send_process(ptr - p_data, _fileSize);
            progress_counter = 0;
        }

        // 优化：一次读取两个字节进行头部匹配
        uint16_t header = (ptr[0] << 8) | ptr[1];
        if (header != valid_header) {
            ptr++;
            continue;
        }

        quint8 msg_type = ptr[2];
        const uchar* payload_ptr = ptr + LOG_PACKET_HEADER_LEN;

        if (msg_type == LOG_FORMAT_MSG)
        {
            if (payload_ptr + 2 > end) { 
                ptr++;
                continue;
            }
            quint8 msg_len = *(payload_ptr + 1);
            if (payload_ptr + msg_len > end) { 
                ptr++;
                continue;
            }

            // 保持原有的缓冲区分配方式
            char name[NAME_LEN+1] = {0};
            char format[FORMAT_LEN+1] = {0};
            char labels[LABELS_LEN+1] = {0};

            const uchar* p = payload_ptr;
            quint8 type = *p++;
            p++; // skip len
            
            // 保持原有的内存拷贝方式
            memcpy(name,       p, NAME_LEN);      p += NAME_LEN;
            memcpy(format,     p, FORMAT_LEN);    p += FORMAT_LEN;
            memcpy(labels,     p, LABELS_LEN);    p += LABELS_LEN;


            // 保持原有的QString构造方式
            QString log_name        = QString(name);
            QString log_format      = QString(format);
            QString log_labels      = QString(labels);

            if (_checkMessage(log_name, log_format, log_labels)) {
                _dataCache->addFormat(type,
                                      log_name,
                                      log_format,
                                      log_labels);
            }
            ptr = payload_ptr + msg_len; 
        }
        else 
        {
            quint8 id = msg_type;
            
            // 优化2：减少重复的isEmpty检查
            LFMT& fmt = FMT[id];
            if (fmt.name.isEmpty() && fmt.format.isEmpty() && fmt.valid) {
                if (APLDataCache::get_singleton()->checkMainTable(id))
                {
                    APLDataCache::get_singleton()->getFormat(id, fmt.name, fmt.format);
                } else {
                    fmt.valid = false;
                }
            }

            if (fmt.valid && !fmt.format.isEmpty()) {
                int msg_len = _getMessageLength(fmt.format);
                if (msg_len < 0 || payload_ptr + msg_len > end) {
                    ptr++;
                    continue;
                }

                // 保持原有的数据添加方式
                _dataCache->addData(fmt.name, fmt.name+QString::number(static_cast<quint8>(*(payload_ptr + 8))), payload_ptr, static_cast<quint8>(*(payload_ptr + 8)), msg_len);
                ptr = payload_ptr + msg_len;

                if (_dataCache->trim_complete) {
                    _dataCache->trim_complete = false;
                    ptr = end;
                }
            } else {
                ptr++;
            }
        }
    }
    emit send_process(_fileSize, _fileSize);
}

// 优化消息长度计算，但保持与原有逻辑完全一致
int APLReadWorker::_getMessageLength(const QString &format) const
{
    // 先检查缓存
    auto it = _formatLengthCache.find(format);
    if (it != _formatLengthCache.end()) {
        return it.value();
    }

    int length = 0;
    QByteArray formatArray = format.toLatin1();
    
    for(char c : formatArray) {
        // 使用查找表优化，但保持原有的switch逻辑作为后备
        int char_length = FORMAT_CHAR_LENGTHS[static_cast<unsigned char>(c)];
        if (char_length > 0) {
            length += char_length;
        } else {
            // 如果查找表中没有，回退到原有的switch逻辑
            switch(c) {
                case 'a': length += 64; break; 
                case 'b':
                case 'B':
                case 'M': length += 1; break;
                case 'h':
                case 'H':
                case 'c':
                case 'C': length += 2; break;
                case 'i':
                case 'I':
                case 'f':
                case 'e':
                case 'E':
                case 'L': length += 4; break;
                case 'd':
                case 'q':
                case 'Q': length += 8; break;
                case 'n': length += 4; break;
                case 'N': length += 16; break;
                case 'Z': length += 64; break;
                default:
                    qCWarning(APLREAD_LOG) << "Unknown format character in length calculation:" << c;
                    return -1; 
            }
        }
    }

    // 缓存结果
    _formatLengthCache[format] = length;
    
    return length;
}

void APLReadWorker::_decodeData(QString &format, const uchar *ptr, QString &value) const
{
    QTextStream st(&value);
    QByteArray formatArray = format.toLatin1();

    for(qint8 i = 0; i< formatArray.count(); i++){
        switch(formatArray[i]){
        case 'a': { 
            qint16 v16_32[32];
            memcpy(v16_32, ptr, sizeof(v16_32));
            ptr += sizeof(v16_32);
            for(int j=0; j<32; j++)
                st << v16_32[j] << (j == 31 ? "" : " ");
            st << ",";
            break;
        }
        case 'b': { 
            qint8 v8;
            memcpy(&v8, ptr, 1); ptr += 1;
            st << v8 << ",";
            break;
        }
        case 'B': { 
            quint8 vu8;
            memcpy(&vu8, ptr, 1); ptr += 1;
            st << vu8 << ",";
            break;
        }
        case 'h': { 
            qint16 v16;
            memcpy(&v16, ptr, 2); ptr += 2;
            st << v16 << ",";
            break;
        }
        case 'H': { 
            quint16 vu16;
            memcpy(&vu16, ptr, 2); ptr += 2;
            st << vu16 << ",";
            break;
        }
        case 'i': { 
            qint32 v32;
            memcpy(&v32, ptr, 4); ptr += 4;
            st << v32 << ",";
            break;
        }
        case 'I': { 
            quint32 vu32;
            memcpy(&vu32, ptr, 4); ptr += 4;
            st << vu32 << ",";
            break;
        }
        case 'f': { 
            float vf;
            memcpy(&vf, ptr, 4); ptr += 4;
            if(qIsNaN(vf) || qIsInf(vf)) vf = 0.0f;
            st << vf << ",";
            break;
        }
        case 'd': { 
            double vd;
            memcpy(&vd, ptr, 8); ptr += 8;
            st << vd << ",";
            break;
        }
        case 'n': { 
            char vc_4[4+1] = {0};
            memcpy(vc_4, ptr, 4); ptr += 4;
            st << "\"" << vc_4 << "\",";
            break;
        }
        case 'N': { 
            char vc_16[16+1] = {0};
            memcpy(vc_16, ptr, 16); ptr += 16;
            st << "\"" << vc_16 << "\",";
            break;
        }
        case 'Z': { 
            char vc_64[64+1] = {0};
            memcpy(vc_64, ptr, 64); ptr += 64;
            st << "\"" << vc_64 << "\",";
            break;
        }
        case 'c': { 
            qint16 v16x100;
            memcpy(&v16x100, ptr, 2); ptr += 2;
            st << (double)(v16x100/100.0f) << ",";
            break;
        }
        case 'C': { 
            quint16 vu16x100;
            memcpy(&vu16x100, ptr, 2); ptr += 2;
            st << (double)(vu16x100/100.0f) << ",";
            break;
        }
        case 'e': { 
            qint32 v32x100;
            memcpy(&v32x100, ptr, 4); ptr += 4;
            st << (double)(v32x100/100.0f) << ",";
            break;
        }
        case 'E': { 
            quint32 vu32x100;
            memcpy(&vu32x100, ptr, 4); ptr += 4;
            st << (double)(vu32x100/100.0f) << ",";
            break;
        }
        case 'L': { 
            qint32 v32l;
            memcpy(&v32l, ptr, 4); ptr += 4;
            st << v32l << ",";
            break;
        }
        case 'M': { 
            quint8 vu8m;
            memcpy(&vu8m, ptr, 1); ptr += 1;
            st << vu8m << ",";
            break;
        }
        case 'q': { 
            qint64 v64;
            memcpy(&v64, ptr, 8); ptr += 8;
            st << v64 << ",";
            break;
        }
        case 'Q': { 
            quint64 vu64;
            memcpy(&vu64, ptr, 8); ptr += 8;
            st << vu64 << ",";
            break;
        }
        }
    }
    if (!value.isEmpty()) {
        value.chop(1);
    }
}

bool APLReadWorker::_checkMessage(QString &name, QString &format, QString &labels) const
{
    bool res = false;
    bool res1, res2, res3;
    res1 = _checkName(name);
    res2 = _checkFormat(format);
    res3 = _checkLabels(labels);
    res = res1 && res2 && res3;
    if(!res){
        qCDebug(APLREAD_LOG) << name << format << labels;
    }
    return res;
}

bool APLReadWorker::_checkName(QString &name) const
{
    // 优化3：避免创建validator对象
    static QRegularExpressionValidator validator(NAME_REGEX, nullptr);

    int pos = 0;
    if(QValidator::Acceptable != validator.validate(name, pos)){
        return false;
    }

    // 优化4：使用QLatin1String避免字符编码转换
    if(name == QLatin1String("FROM")) {
        name = QLatin1String("`FROM`");
    }
    else if(name == QLatin1String("TO")) {
        name = QLatin1String("`TO`");
    }

    return true;
}

bool APLReadWorker::_checkFormat(QString &format) const
{
    // 优化3：避免创建validator对象
    static QRegularExpressionValidator validator(FORMAT_REGEX, nullptr);

    int pos = 0;
    if(QValidator::Acceptable != validator.validate(format, pos)){
        return false;
    }

    return true;
}

bool APLReadWorker::_checkLabels(QString &labels) const
{
    // 优化3：避免创建validator对象
    static QRegularExpressionValidator validator(LABELS_REGEX, nullptr);

    int pos = 0;
    if(QValidator::Acceptable != validator.validate(labels, pos)){
        return false;
    }

    int index_of_Primary = labels.indexOf(QLatin1String("Primary"), 0, Qt::CaseInsensitive);
    if( index_of_Primary != -1 ) {
        labels.insert(index_of_Primary+7, QLatin1Char('`'));
        labels.insert(index_of_Primary, QLatin1Char('`'));
        qCDebug(APLREAD_LOG) <<labels;
    }

    int index_of_Limit = labels.indexOf(QLatin1String("Limit"), 0, Qt::CaseInsensitive);
    if(index_of_Limit != -1) {
        labels.insert(index_of_Limit+5, QLatin1Char('`'));
        labels.insert(index_of_Limit, QLatin1Char('`'));
        qCDebug(APLREAD_LOG) <<labels;
    }

    int index_of_Default = labels.indexOf(QLatin1String("Default"), 0, Qt::CaseInsensitive);
    if(index_of_Default != -1) {
        labels.insert(index_of_Default+7, QLatin1Char('`'));
        labels.insert(index_of_Default, QLatin1Char('`'));
        qCDebug(APLREAD_LOG) <<labels;
    }

    int index_of_IS = labels.indexOf(QLatin1String(",IS"), 0, Qt::CaseInsensitive);
    if(index_of_IS != -1) {
        labels.insert(index_of_IS+3, QLatin1Char('`'));
        labels.insert(index_of_IS+1, QLatin1Char('`'));
        qCDebug(APLREAD_LOG) <<labels;
    }

    int index_of_As = labels.indexOf(QLatin1String("As,"), 0, Qt::CaseInsensitive);
    if(index_of_As != -1) {
        labels.insert(index_of_As+2, QLatin1Char('`'));
        labels.insert(index_of_As, QLatin1Char('`'));
        qCDebug(APLREAD_LOG) <<labels;
    }

    int index_of_As1 = labels.indexOf(QLatin1String("AS"), 0, Qt::CaseSensitive);
    if(index_of_As1 != -1) {
        labels.insert(index_of_As1+2, QLatin1Char('`'));
        labels.insert(index_of_As1, QLatin1Char('`'));
        qCDebug(APLREAD_LOG) <<labels;
    }

    int index_of_index = labels.indexOf(QLatin1String("index"), 0, Qt::CaseSensitive);
    if(index_of_index != -1) {
        labels.insert(index_of_index+5, QLatin1Char('`'));
        labels.insert(index_of_index, QLatin1Char('`'));
        qCDebug(APLREAD_LOG) <<labels;
    }
    return true;
}

APLReadWorker::APLReadWorker(QObject *parent)
    : QObject(parent)
    , _dataCache(new APLDataCache)
    , _fileSize(0)
{

    // 初始化格式字符长度查找表
    initFormatCharLengths();
}

APLReadWorker::~APLReadWorker()
{
    delete _dataCache;
}

void APLReadWorker::decodeLogFile(const QString &file_dir)
{
    QFile  file;

    file.setFileName(file_dir);
    if(!file.open(QIODevice::ReadOnly))
    {
        qCDebug(APLREAD_LOG)<< "can not open file!";
        return;
    }

    _fileSize = file.size();
    uchar* fpr = file.map(0, _fileSize);
    if(fpr){
        _decode(fpr, _fileSize);

        QFileInfo fileInfo(file_dir);

        emit fileOpened();
        qCDebug(APLREAD_LOG) << "All data have been read";

        file.unmap(fpr);
    }
    file.close();

    QFileInfo fileInfo(file_dir);
    _dataCache->exportToFile(fileInfo.absolutePath() + "/" + fileInfo.baseName() + "_csv");
}
