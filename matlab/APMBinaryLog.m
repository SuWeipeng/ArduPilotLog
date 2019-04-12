function APMBinaryLog
    eval('clc');
    disp('clear all');
    disp('waiting ...');
    LOG_FORMAT_MSG    = 128;
    
    % #define HEAD_BYTE1      0xA3    // Decimal 163
    % #define HEAD_BYTE2      0x95    // Decimal 149
    HEAD_BYTE1 = 163;
    HEAD_BYTE2 = 149;
    
    % struct LogStructure {
    %     uint8_t msg_type;
    %     uint8_t msg_len;
    %     const char name[5];
    %     const char format[16];
    %     const char labels[64];
    % };
    NameLength   = 4;
    FormatLength = 16;
    LabelLength  = 64;

    fid = -1;
    [filename, pathname] = uigetfile({'*.db';'*.bin';'*.*'}, 'Open log file');
    fullname             = strcat(char(pathname), char(filename));
    if isempty(fullname)
        return; 
    end
    split_file_name = regexp(filename, '\.', 'split');
    if strcmp(split_file_name(1, size(split_file_name,2)), 'db') == 1
        disp('Do not recognize Chinese path !!!');
        conn = sqlite(fullname, 'readonly');
        group_name = fetch(conn,'SELECT name FROM maintable');
        labels = fetch(conn,'SELECT labels FROM maintable');
        names={};
        tic;
        for i = 1:size(group_name, 1)
            valid_group = cell2mat(eval(strcat('fetch(conn,','''SELECT COUNT(*) FROM', 32, char(group_name{i}),''')')));
            if valid_group == 0
                continue;
            else
                eval(strcat('content=', 'fetch(conn,','''SELECT * FROM', 32, char(group_name{i}),''');'));
                C={}; FIELDS={};
                colums = size(content,2);
                for j = 1:colums
                    if isnumeric(content{1, j})
                        C{j} = double(cell2mat(content(:, j)));
                    else
                        C{j} = content(:, j);
                    end
                end
                FIELDS = regexp(char(labels(i,1)), ',', 'split');
                C = C(:,2:size(C,2));
                eval(strcat(lower(char(group_name{i})),'=cell2struct(C,FIELDS,2);'));
                eval(char(strcat('assignin(','''','base','''',', ','''',char(lower(char(group_name{i}))),'''',',', char(lower(char(group_name{i}))), ');')));
                names  {numel(names)  +1, 1} = lower(char(group_name{i}));
            end
        end
        close(conn);
        toc;
        assignin('base','names',sort(names));
        clear C colums content group_name i j labels valid_group FIELDS;
        
    else

        fid = fopen(fullname, 'rb');
        if fid == -1 
            return; 
        end
        fsize = 0;
        if(fid > 0)
            fseek(fid, 0, 1);
            fsize = ftell(fid);
            fseek(fid, 0, -1);
        end

        types={}; lengths={}; names={}; formats={}; labels={};values={};log={};
        c_last  = zeros(3, 1);

    %     tic; % tic1
        % first read find all FMT
        while ftell(fid) < fsize
            c       = fread(fid, 1);
            ptr_pos = ftell(fid);
            if ptr_pos > 3
                c_last(1) = c_last(2);
                c_last(2) = c_last(3);
                c_last(3) = c;
            else 
                switch ptr_pos
                    case 1
                        c_last(1) = c;
                        continue;
                    case 2
                        c_last(2) = c;
                        continue;
                    case 3
                        c_last(3) = c; 
                end
            end

            if isequal(c_last(1:2, 1),[HEAD_BYTE1; HEAD_BYTE2])
                if c_last(3) == LOG_FORMAT_MSG % Find FMT format infomation
                    type   = fread(fid, 1);
                    length = fread(fid, 1);
                    name   = strtrim(cellstr(char(fread(fid, NameLength)')));
                    format = strtrim(cellstr(char(fread(fid, FormatLength)')));
                    label  = strtrim(strcat(cellstr(char(fread(fid, LabelLength)')), ',n'));
                    value  = {};
                    if any(strcmp(name,names))
                        continue;
                    else
                        types  {numel(types)  +1, 1} = type;
                        lengths{numel(lengths)+1, 1} = length;
                        names  {numel(names)  +1, 1} = name;
                        formats{numel(formats)+1, 1} = format;
                        labels {numel(labels) +1, 1} = char2cell(char(label),[',']);
                        values {numel(values) +1, 1} = value;
                        log = [types, lengths, names, formats, labels, values];
                    end
                else
                    continue;
                end
            end
        end

    %     eval('clc');
    %     disp(['first read time consuming:', num2str(toc),'(s)']); 
    %     tic; % tic2

        % re-read log file
        fseek(fid, 0, -1);
        num = 0;

        while ftell(fid) < fsize
            c       = fread(fid, 1);
            ptr_pos = ftell(fid);
            if ptr_pos > 3
                c_last(1) = c_last(2);
                c_last(2) = c_last(3);
                c_last(3) = c;
            else 
                switch ptr_pos
                    case 1
                        c_last(1) = c;
                        continue;
                    case 2
                        c_last(2) = c;
                        continue;
                    case 3
                        c_last(3) = c; 
                end
            end

            if isequal(c_last(1:2, 1),[HEAD_BYTE1; HEAD_BYTE2])
                if c_last(3) == LOG_FORMAT_MSG % Find FMT format infomation
                    continue;
                end
                row_num = find(c_last(3)==cell2mat(log(:,1))); 
                if ~isempty(row_num) % If the ID already in the list
                    format = strtrim(cellstr(log{row_num, 4}));
                    format_str = format{1};

                    format_len = size(format_str, 2);
                    value      = {};
                    for i = 1: format_len
                        switch format_str(i)
                            case 'b'                             %   b   : int8_t
                                value{i} = fread(fid, 1, 'int8');
                                if isempty(value{i})
                                    value{i} = 0;
                                end
                            case 'B'                             %   B   : uint8_t
                                value{i} = fread(fid, 1, 'uint8');
                                if isempty(value{i})
                                    value{i} = 0;
                                end
                            case 'h'                             %   h   : int16_t
                                value{i} = fread(fid, 1, 'int16');
                                if isempty(value{i})
                                    value{i} = 0;
                                end
                            case 'H'                             %   H   : uint16_t
                                value{i} = fread(fid, 1, 'uint16');
                                if isempty(value{i})
                                    value{i} = 0;
                                end
                            case 'i'                             %   i   : int32_t
                                value{i} = fread(fid, 1, 'int32');
                                if isempty(value{i})
                                    value{i} = 0;
                                end
                            case 'I'                             %   I   : uint32_t
                                value{i} = fread(fid, 1, 'uint32');
                                if isempty(value{i})
                                    value{i} = 0;
                                end
                            case 'f'                             %   f   : float
                                value{i} = fread(fid, 1, 'single');
                                if isempty(value{i})
                                    value{i} = 0;
                                end
                            case 'd'                             %   d   : double
                                value{i} = fread(fid, 1, 'double');
                                if isempty(value{i})
                                    value{i} = 0;
                                end
                            case 'n'                             %   n   : char[4]
                                value{i} = strtrim(cellstr(fread(fid, 4, 'uint8=>char')'));
                            case 'N'                             %   N   : char[16]
                                value{i} = strtrim(cellstr(fread(fid, 16, 'uint8=>char')'));
                            case 'Z'                             %   Z   : char[64]
                                value{i} = strtrim(cellstr(fread(fid, 64, 'uint8=>char')'));
                            case 'c'                             %   c   : int16_t * 100
                                value{i} = fread(fid, 1, 'int16')/100;
                                if isempty(value{i})
                                    value{i} = 0;
                                end
                            case 'C'                             %   C   : uint16_t * 100
                                value{i} = fread(fid, 1, 'uint16')/100;
                                if isempty(value{i})
                                    value{i} = 0;
                                end
                            case 'e'                             %   e   : int32_t * 100
                                value{i} = fread(fid, 1, 'int32')/100;
                                if isempty(value{i})
                                    value{i} = 0;
                                end
                            case 'E'                             %   E   : uint32_t * 100
                                value{i} = fread(fid, 1, 'uint32')/100;
                                if isempty(value{i})
                                    value{i} = 0;
                                end
                            case 'L'                             %   L   : int32_t latitude/longitude
                                value{i} = fread(fid, 1, 'int32');
                                if isempty(value{i})
                                    value{i} = 0;
                                end
                            case 'M'                             %   M   : uint8_t flight mode
                                value{i} = fread(fid, 1, 'uint8');
                                if isempty(value{i})
                                    value{i} = 0;
                                end
                            case 'q'                             %   q   : int64_t
                                value{i} = fread(fid, 1, 'int64');
                                if isempty(value{i})
                                    value{i} = 0;
                                end
                            case 'Q'                             %   Q   : uint64_t
                                value{i} = fread(fid, 1, 'uint64');
                                if isempty(value{i})
                                    value{i} = 0;
                                end
                        end
                    end
                    num        = num +1;
                    value{i+1} = num;
                    log{row_num, 6}(size(log{row_num, 6},1)+1,:) = value; 
                end
            end
        end
        fclose(fid);

    %     disp(['second read time consuming:', num2str(toc),'(s)']); 
    %     tic; % tic3

        val_names={};
        for i = 1: size(log,1)
            if isempty(log{i, 6})
                continue;
            else
                C={}; FIELDS={};
                for j = 1: size(log{i, 5},1)
                    C{j}=log{i, 6}(:, j);
                    FIELDS{j}=char(log{i, 5}(j));
                end
                eval(strcat(lower(char(log{i, 3})),'=cell2struct(C,FIELDS,2);'));
                if ~isequal(char(log{i, 3}), 'PARM') && ~isequal(char(log{i, 3}), 'MSG')
                    val_names{numel(val_names)+1, 1} = lower(char(log{i, 3}));
                end
                log{i, 7}(1,1) = strfind(log{i, 4}, 'n');
                log{i, 7}(1,2) = strfind(log{i, 4}, 'N');
                log{i, 7}(1,3) = strfind(log{i, 4}, 'Z');
                for j = 1: size(char(log{i, 4}), 2)+1 % Additional num
                    if ~isempty(find(j == cell2mat(log{i, 7}(1,1))))
                        continue;
                    else if ~isempty(find(j == cell2mat(log{i, 7}(1,2))))
                            continue;
                        else if ~isempty(find(j == cell2mat(log{i, 7}(1,3))))
                                continue;
                            else
                                eval(char(strcat(lower(log{i, 3}),'.',char(log{i, 5}(j, 1)),' = cell2mat(',lower(log{i, 3}),'.', char(log{i, 5}(j, 1)),');')));
                            end
                        end
                    end
                end
            end
        end

    %     disp(['Arrangement data time consuming:', num2str(toc),'(s)']); 
    %     tic; % tic4

        arm_time_us    = 0;
        disarm_time_us = 0;
        if exist('ev')
            for i = 1: size(ev.TimeUS, 1)
                if isequal(ev.Id(i), 10)
                    arm_time_us = ev.TimeUS(i);
                end   
                if isequal(ev.Id(i), 11) || isequal(ev.Id(i), 17) || isequal(ev.Id(i), 18)
                    disarm_time_us = ev.TimeUS(i);
                end
            end
        end

        find_time_end_by_rcou = false;
        time_ref              = 0;
        if exist('rcou')
            if ~isempty(find(contains(fieldnames(rcou),'C1')==1))
                for i = 1: size(rcou.TimeUS, 1)
                    if ~isequal(rcou.C1(i), rcou.C2(i), rcou.C3(i), rcou.C4(i))
                        time_ref = rcou.TimeUS(i);
                    end
                    if isequal(rcou.C1(i), rcou.C2(i), rcou.C3(i), rcou.C4(i)) && ~find_time_end_by_rcou && time_ref > 0
                        disarm_time_us        = rcou.TimeUS(i);
                        find_time_end_by_rcou = true;
                    end
                end
            end
        end

        fly_time_us  = disarm_time_us - arm_time_us;
        fly_time_min = floor(fly_time_us/(60 * 1e6));
        fly_time_s   = fly_time_us/1e6 - fly_time_min * 60;
        eval('clc');
        if fly_time_us > 0
            disp(['fly time: ', num2str(fly_time_min),' min ', num2str(fly_time_s), ' s']);
        else
            disp(['can not find end timestamp']);
        end

    %     disp(['calc fly time cost:', num2str(toc),'(s)']); 

        % if this script run as a fuction, then below is useful.
        for i = 1: size(log, 1)
            if isempty(log{i, 6})
                continue;
            else
                eval(char(strcat('assignin(','''','base','''',', ','''',char(lower(char(log{i, 3}))),'''',',', char(lower(char(log{i, 3}))), ');')));
            end
        end
        assignin('base','log',log);
        assignin('base','names',sort(val_names));
    end
end
