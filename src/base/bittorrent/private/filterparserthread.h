#ifndef FILTERPARSERTHREAD_H
#define FILTERPARSERTHREAD_H

#include <QThread>

#include <libtorrent/ip_filter.hpp>

class QDataStream;

class FilterParserThread : public QThread
{
    Q_OBJECT

public:
    FilterParserThread(QObject *parent = nullptr);
    ~FilterParserThread();
    void processFilterFile(const QString &filePath);
    libtorrent::ip_filter IPfilter();

signals:
    void IPFilterParsed(int ruleCount);
    void IPFilterError();

protected:
    void run();

private:
    int findAndNullDelimiter(char *const data, char delimiter, int start, int end, bool reverse = false);
    int trim(char *const data, int start, int end);
    int parseDATFilterFile();
    int parseP2PFilterFile();
    int getlineInStream(QDataStream &stream, std::string &name, char delim);
    int parseP2BFilterFile();

    bool m_abort;
    QString m_filePath;
    libtorrent::ip_filter m_filter;
};

#endif // BITTORRENT_FILTERPARSERTHREAD_H
