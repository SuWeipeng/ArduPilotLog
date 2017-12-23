#ifndef APLLOGGINGCATEGORY_H
#define APLLOGGINGCATEGORY_H

#include <QLoggingCategory>
#include <QStringList>

Q_DECLARE_LOGGING_CATEGORY(MainWindow)

/// @def APL_LOGGING_CATEGORY
/// This is a APL specific replacement for Q_LOGGING_CATEGORY. It will register the category name into a
/// global list. It's usage is the same as Q_LOGGING_CATEOGRY.
#define APL_LOGGING_CATEGORY(name, ...) \
    static APLLoggingCategory aplCategory ## name (__VA_ARGS__); \
    Q_LOGGING_CATEGORY(name, __VA_ARGS__)

class APLLoggingCategoryRegister : public QObject
{
    Q_OBJECT

public:
    static APLLoggingCategoryRegister* instance(void);

    /// Registers the specified logging category to the system.
    void registerCategory(const char* category) { _registeredCategories << category; }

    /// Returns the list of available logging category names.
    Q_INVOKABLE QStringList registeredCategories(void);

    /// Turns on/off logging for the specified category. State is saved in app settings.
    Q_INVOKABLE void setCategoryLoggingOn(const QString& category, bool enable);

    /// Returns true if logging is turned on for the specified category.
    Q_INVOKABLE bool categoryLoggingOn(const QString& category);

    /// Sets the logging filters rules from saved settings.
    ///     @param commandLineLogggingOptions Logging options which were specified on the command line
    void setFilterRulesFromSettings(const QString& commandLineLoggingOptions);

private:
    APLLoggingCategoryRegister(void) { }

    QStringList _registeredCategories;
    QString     _commandLineLoggingOptions;

    static const char* _filterRulesSettingsGroup;
};

class APLLoggingCategory
{
public:
    APLLoggingCategory(const char* category) { QGCLoggingCategoryRegister::instance()->registerCategory(category); }
};

#endif // APLLOGGINGCATEGORY_H
