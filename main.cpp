#include <QCoreApplication>
#include <QFile>
#include <QDebug>
#include <QString>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

bool DEBUG_MODE;

int getIndexOfMaxValue(const QList<int>& intList)
{
    int maxValue = -1;
    int maxValueIndex = 0;

    for (int i = 0; i < intList.length(); ++i)
    {
        if (intList.at(i) > maxValue)
        {
            maxValue = intList.at(i);
            maxValueIndex = i;
        }
    }

    return maxValueIndex;
}

int getIndexOfMinValue(const QList<int>& intList)
{
    int minValue = 99999999;
    int minValueIndex = 0;

    for (int i = 0; i < intList.length(); ++i)
    {
        if (intList.at(i) < minValue)
        {
            minValue = intList.at(i);
            minValueIndex = i;
        }
    }

    return minValueIndex;
}

void swapItemsInList(QByteArray& array, int indexFrom, int indexTo)
{
    if (indexFrom == indexTo)
        return;

    int buffer = array.at(indexTo);
    array[indexTo] = array[indexFrom];
    array[indexFrom] = buffer;
}

void getGraphicKey(const QList<int>& key)
{
    qDebug() << "";
    qDebug() << "---------Graphic key model-----------";
    qDebug() << "*************************************";

    QString result;

    int keyLength = key.length();
    QList<int> tempKey = key;

    int maxValue = key.at(getIndexOfMaxValue(key));
    int minValue = key.at(getIndexOfMinValue(key));

    for (int i = maxValue; i >= minValue; --i)
    {
        int j = 0;

        while (j < keyLength)
        {
            int index = getIndexOfMaxValue(tempKey);
            if (key.at(index) != i)
            {
                for (int k = 0; k < keyLength - j; ++k)
                    result.append("-");

                break;
            }

            if (index != j)
                result.append("-");
            else
            {
                tempKey[index] = -1;
                result.append("O");
            }

            ++j;
        }

        qDebug() << i << result;
        result.clear();
    }

    qDebug() << "*************************************";
    qDebug() << "";
}

bool decode(const QString& inputFileName, const QList<int>& key, const QString& ouptutFileName)
{
    QFile file(inputFileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Failed to open a file" << file.errorString();
        return false;
    }

    QFile decodedFile(ouptutFileName);
    if (!decodedFile.open(QIODevice::WriteOnly))
    {
        qDebug() << "Failed to open a file" << decodedFile.errorString();
        return false;
    }

    int blockLength = key.length();
    while (!file.atEnd())
    {
        QByteArray encryptedBlock = file.read(blockLength);
        QByteArray sourceBlock = encryptedBlock;

        if (DEBUG_MODE)
            qDebug() << "Decrypting block" << encryptedBlock;

        QList<int> tempKey = key;
        for (int i = 0; i < blockLength; ++i)
        {
            int index = getIndexOfMaxValue(tempKey);
            tempKey[index] = -1;
            encryptedBlock[index] = sourceBlock.at(i);
        }

        if (DEBUG_MODE)
            qDebug() << "  result: " << encryptedBlock;

        decodedFile.write(encryptedBlock);
    }

    file.close();
    decodedFile.close();

    qDebug() << "Done decoding";
    return true;
}

bool encode(const QString& inputFileName, const QList<int>& key, const QString& ouptutFileName)
{
    QFile file(inputFileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Failed to open a file" << file.errorString();
        return false;
    }

    QFile encodedFile(ouptutFileName);
    if (!encodedFile.open(QIODevice::WriteOnly))
    {
        qDebug() << "Failed to open a file" << encodedFile.errorString();
        return false;
    }

    int blockLength = key.length();
    while (!file.atEnd())
    {
        QByteArray encryptedBlock = file.read(blockLength);
        QByteArray sourceBlock = encryptedBlock;

        if (DEBUG_MODE)
            qDebug() << "Encrypting block" << encryptedBlock;

        for (int i = 0; i < blockLength - encryptedBlock.length(); ++i)
            encryptedBlock.append(" ");

        QList<int> tempKey = key;
        for (int i = 0; i < blockLength; ++i)
        {
            int index = getIndexOfMaxValue(tempKey);
            tempKey[index] = -1;
            encryptedBlock[i] = sourceBlock.at(index);
        }

        if (DEBUG_MODE)
            qDebug() << "  result: " << encryptedBlock;

        encodedFile.write(encryptedBlock);
    }

    file.close();
    encodedFile.close();

    qDebug() << "Done encoding.";
    return true;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QCoreApplication::setApplicationName("Info Sec Lab 1");
    QCoreApplication::setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("Test helper");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("source", QCoreApplication::translate("main", "Source file to copy."));
    parser.addPositionalArgument("destination", QCoreApplication::translate("main", "Destination directory."));

    QCommandLineOption decryptOption("d", QCoreApplication::translate("main", "Decrypt a file"));
    parser.addOption(decryptOption);

    QCommandLineOption encryptOption("e", QCoreApplication::translate("main", "Enrypt a file"));
    parser.addOption(encryptOption);

    QCommandLineOption outputFileOption(QStringList() << "o" << "output-file",
            QCoreApplication::translate("main", "Create an encoded file with name <directory>."),
            QCoreApplication::translate("main", "file"));
    parser.addOption(outputFileOption);

    QCommandLineOption inputFileOption(QStringList() << "i" << "input-file",
            QCoreApplication::translate("main", "Create an encoded from file with name <directory>."),
            QCoreApplication::translate("main", "directory"));
    parser.addOption(inputFileOption);

    QCommandLineOption keyFileOption(QStringList() << "k" << "key-file",
            QCoreApplication::translate("main", "Create an encoded from file using key file <directory>."),
            QCoreApplication::translate("main", "directory"));
    parser.addOption(keyFileOption);

    QCommandLineOption debugModeOption("t", QCoreApplication::translate("main", "Decrypt a file"));
    parser.addOption(debugModeOption);

    parser.process(app);

    bool decrypt = parser.isSet(decryptOption);
    bool encrypt = parser.isSet(encryptOption);
    QString outputFileName = parser.value(outputFileOption);
    QString inputFileName = parser.value(inputFileOption);
    QString keyFileName = parser.value(keyFileOption);
    DEBUG_MODE = parser.isSet(debugModeOption);

    if ((decrypt && encrypt) || (!encrypt && ! decrypt))
    {
        qDebug() << "Please check decrypt/ecnrypt parameter.";
        return 1;
    }


    QFile keyFile(keyFileName);
    if (!keyFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "Failed to open key file." << keyFile.errorString();
        return 1;
    }

    qDebug() << "Reading key file...";
    QList<int> key;
    QJsonArray keyValuesArray = QJsonDocument::fromJson(keyFile.readAll()).object().value("key").toArray();;
    for (int i = 0; i < keyValuesArray.size(); ++i)
        key.append(keyValuesArray.at(i).toInt());

    qDebug() << "Done reading key file.";

    if (DEBUG_MODE)
        getGraphicKey(key);

    if (encrypt)
    {
        if (!encode(inputFileName, key, outputFileName))
            return 1;
    }
    else if (decrypt)
    {
        if (!decode(inputFileName, key, outputFileName))
            return 1;
    }

    return 0;
}

