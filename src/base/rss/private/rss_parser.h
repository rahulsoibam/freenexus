#pragma once

#include <QList>
#include <QObject>
#include <QString>
#include <QVariantHash>

class QXmlStreamReader;

namespace RSS
{
    namespace Private
    {
        struct ParsingResult
        {
            QString error;
            QString lastBuildDate;
            QString title;
            QList<QVariantHash> articles;
        };

        class Parser: public QObject
        {
            Q_OBJECT

        public:
            explicit Parser(QString lastBuildDate);
            void parse(const QByteArray &feedData);

        signals:
            void finished(const RSS::Private::ParsingResult &result);

        private:
            Q_INVOKABLE void parse_impl(const QByteArray &feedData);
            void parseRssArticle(QXmlStreamReader &xml);
            void parseRSSChannel(QXmlStreamReader &xml);
            void parseAtomArticle(QXmlStreamReader &xml);
            void parseAtomChannel(QXmlStreamReader &xml);

            QString m_baseUrl;
            ParsingResult m_result;
        };
    }
}

Q_DECLARE_METATYPE(RSS::Private::ParsingResult)
