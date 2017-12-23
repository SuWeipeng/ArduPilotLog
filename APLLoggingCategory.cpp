#include "APLLoggingCategory.h"

#include <QSettings>

// Add Global logging categories (not class specific) here using APL_LOGGING_CATEGORY
APL_LOGGING_CATEGORY(FirmwareUpgradeLog,        "FirmwareUpgradeLog")

APLLoggingCategoryRegister* _instance = NULL;
const char* APLLoggingCategoryRegister::_filterRulesSettingsGroup = "LoggingFilters";

APLLoggingCategoryRegister* APLLoggingCategoryRegister::instance(void)
{
    if (!_instance) {
        _instance = new APLLoggingCategoryRegister();
        Q_CHECK_PTR(_instance);
    }

    return _instance;
}

QStringList APLLoggingCategoryRegister::registeredCategories(void)
{
    _registeredCategories.sort();
    return _registeredCategories;
}

void APLLoggingCategoryRegister::setCategoryLoggingOn(const QString& category, bool enable)
{
    QSettings settings;

    settings.beginGroup(_filterRulesSettingsGroup);
    settings.setValue(category, enable);
}

bool APLLoggingCategoryRegister::categoryLoggingOn(const QString& category)
{
    QSettings settings;

    settings.beginGroup(_filterRulesSettingsGroup);
    return settings.value(category, false).toBool();
}

void APLLoggingCategoryRegister::setFilterRulesFromSettings(const QString& commandLineLoggingOptions)
{
    if (!commandLineLoggingOptions.isEmpty()) {
        _commandLineLoggingOptions = commandLineLoggingOptions;
    }
    QString filterRules;

    // Turn off bogus ssl warning
    filterRules += "qt.network.ssl.warning=false\n";
    filterRules += "*Log.debug=false\n";

    // Set up filters defined in settings
    foreach (QString category, _registeredCategories) {
        if (categoryLoggingOn(category)) {
            filterRules += category;
            filterRules += ".debug=true\n";
        }
    }

    // Command line rules take precedence, so they go last in the list
    if (!_commandLineLoggingOptions.isEmpty()) {
        QStringList logList = _commandLineLoggingOptions.split(",");

        if (logList[0] == "full") {
            filterRules += "*Log.debug=true\n";
            for(int i=1; i<logList.count(); i++) {
                filterRules += logList[i];
                filterRules += ".debug=false\n";
            }
        } else {
            foreach(const QString &rule, logList) {
                filterRules += rule;
                filterRules += ".debug=true\n";
            }
        }
    }

    qDebug() << "Filter rules" << filterRules;
    QLoggingCategory::setFilterRules(filterRules);
}
