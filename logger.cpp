#include "logger.h"


logger& logger::instance(){
    // The static keyword here ensures it is created once and lives until program termination
    static logger loggerInstance;
    return loggerInstance;
}

void logger::initalize(QDockWidget* consoleDock)
{
    //Stores reference to  the console output area (defined as consoleOutput in Ui code)
    consoleOutput = consoleDock->findChild<QTextEdit*>("consoleOutput");
}

void logger::logmsg(const QString& Message, logType Type)
{
    if(!consoleOutput)
        return;

    QString Severity;
    QString colorCode;

    //Gets the current time
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");

    //Changes the color and input based on enum

    switch (Type) {
    case logType::LogTemp:
        Severity = "Info";
        colorCode = "lightgrey";
        break;

    case logType::Critical:
        Severity = "Critical";
        colorCode = "orange";
        break;

    case logType::Error:
        Severity = "Error";
        colorCode = "brightred";
        break;

    case logType::Warning:
        Severity = "[Warning]";
        colorCode = "Yellow";
        break;

    }

    /*Text color, timestamp, severity and message is packaged as html with css to style it
     * the format is accepted by QTextEdit and will be processed appropriatly.
     * output example:
     *          [info] 09:34:25 program startes as expected
     */
    QString htmlFormatedOutput = QString("<span style=\"color:%1;\">%2: [%3] %4</span>")
                       .arg(colorCode, timestamp, Severity, Message);
                        //      %1        %2         %3       %4

    //Sent to the Ui container for output
    consoleOutput->append(htmlFormatedOutput);
}

void logger::logmsg(const QString& Message, QString Color)
{
    if(!consoleOutput)return;

    //Gets the current time
    QString timeStamp = QDateTime::currentDateTime().toString("hh:mm:ss");

    //Packages log as a html output                                         %1      %2         %3
    QString html = QString("<span style=\"color:%1;\">%2: %3</span>").arg(Color,timeStamp, Message);

    //Sends log to Ui for output
    consoleOutput->append(html);
}
