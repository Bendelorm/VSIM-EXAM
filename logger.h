#ifndef LOGGER_H
#define LOGGER_H


   /*
    *  Using Singleton Design Pattern to ensure only one logger exists in the application
    *
    * A single instance of a logger is created in MainWindow.cpp
    * with a reference to the console log from the Ui attached.
    * End-goal is to create a logging system that can be accessed anywhere,
    * without the need to include or reference anything.
    *
    * Basically recreating logging like UE or Unity, but ours is cooler.
    *
    */


#include <QString>
#include <QDockWidget>
#include <QTextEdit>
#include <QDateTime>

   ///
   /// @brief Levels of severity for to catogorize logs
   ///
enum class logType{
        LogTemp,
        Warning,
        Error,
        Critical

};

class logger
{
public:
    ///
    /// @brief A single logger instance is created for logging purposes
    /// @return instance of logger
    ///
    static logger& instance();

    ///
    /// @brief Creates a reference to the console-output in the UI
    /// @param QDockWidget - The container of the console log
    ///
    void initalize(QDockWidget* consoleDock);


    ///
    /// @brief
    /// Passes a timestamped message to the console log, with option of adding level of severity
    /// @param String message
    /// @param enum Type - Severity of message, default to Logtemp
    /// @details
    ///    Types: logType::LogTemp, logType::Warning, logType::Error, logType::Critical
    ///
    void logmsg(const QString& Message, logType Type = logType::LogTemp);


    ///
    /// @brief
    /// Passes a timestamped mmessage to the console log with option for custom text color
    ///
    /// @param String Message
    /// @param String color -> Default lightgrey
    /// \details If no color is provided, defaults to lightgrey
    ///
    void logmsg(const QString& Message, QString Color = "lightgrey");
private:

    logger() = default; // Default and private constructor to prevent instantiation

    /*Delete copy constructor and assignment operator*/
    // This way it cannot be copied nor assigned to a different object
    logger(const logger&) = delete;
    logger& operator=(const logger&) = delete;

    //Reference to output destination. The actual console log.
    QTextEdit* consoleOutput{nullptr};

};

#endif // LOGGER_H




