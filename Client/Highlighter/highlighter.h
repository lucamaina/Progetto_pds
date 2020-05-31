#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QVector>

class Highlighter: public QSyntaxHighlighter
{
public:
    Highlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule{
        QRegularExpression pattern;
        QTextCharFormat format;
    };

QVector<HighlightingRule> HighlightingRules;

QRegularExpression CommentStart;
QRegularExpression CommentEnd;

QTextCharFormat keywordFormat;
QTextCharFormat classFormat;
QTextCharFormat singleLineCommentFormat;
QTextCharFormat multiLineCommentFormat;
QTextCharFormat quotationFormat;
QTextCharFormat functionFormat;

};

#endif // HIGHLIGHTER_H
