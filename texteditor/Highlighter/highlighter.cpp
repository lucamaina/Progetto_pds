#include "highlighter.h"

Highlighter::Highlighter(QTextDocument *parent): QSyntaxHighlighter(parent)
{
    HighlightingRule Rule;

    keywordFormat.setForeground(Qt::blue);

    const QString keywordPatterns[] = {
            QStringLiteral("\\bchar\\b"), QStringLiteral("\\bclass\\b"), QStringLiteral("\\bconst\\b"),
            QStringLiteral("\\bdouble\\b"), QStringLiteral("\\benum\\b"), QStringLiteral("\\bexplicit\\b"),
            QStringLiteral("\\bfriend\\b"), QStringLiteral("\\binline\\b"), QStringLiteral("\\bint\\b"),
            QStringLiteral("\\blong\\b"), QStringLiteral("\\bnamespace\\b"), QStringLiteral("\\boperator\\b"),
            QStringLiteral("\\bprivate\\b"), QStringLiteral("\\bprotected\\b"), QStringLiteral("\\bpublic\\b"),
            QStringLiteral("\\bshort\\b"), QStringLiteral("\\bsignals\\b"), QStringLiteral("\\bsigned\\b"),
            QStringLiteral("\\bslots\\b"), QStringLiteral("\\bstatic\\b"), QStringLiteral("\\bstruct\\b"),
            QStringLiteral("\\btemplate\\b"), QStringLiteral("\\btypedef\\b"), QStringLiteral("\\btypename\\b"),
            QStringLiteral("\\bunion\\b"), QStringLiteral("\\bunsigned\\b"), QStringLiteral("\\bvirtual\\b"),
            QStringLiteral("\\bvoid\\b"), QStringLiteral("\\bvolatile\\b"), QStringLiteral("\\bbool\\b")
        };
        for (const QString &pattern : keywordPatterns) {
            Rule.pattern = QRegularExpression(pattern);
            Rule.format = keywordFormat;
            HighlightingRules.append(Rule);
        }

        classFormat.setFontWeight(QFont::Bold);
           classFormat.setForeground(Qt::darkMagenta);
           Rule.pattern = QRegularExpression(QStringLiteral("\\bQ[A-Za-z]+\\b"));
           Rule.format = classFormat;
           HighlightingRules.append(Rule);

           quotationFormat.setForeground(Qt::darkGreen);
           Rule.pattern = QRegularExpression(QStringLiteral("\".*\""));
           Rule.format = quotationFormat;
           HighlightingRules.append(Rule);

           functionFormat.setFontItalic(true);
           functionFormat.setForeground(Qt::darkCyan);
           Rule.pattern = QRegularExpression(QStringLiteral("\\b[A-Za-z0-9_]+(?=\\()"));
           Rule.format = functionFormat;
           HighlightingRules.append(Rule);

           singleLineCommentFormat.setForeground(Qt::red);
           Rule.pattern = QRegularExpression(QStringLiteral("//[^\n]*"));
           Rule.format = singleLineCommentFormat;
           HighlightingRules.append(Rule);

           multiLineCommentFormat.setForeground(Qt::red);

           CommentStart = QRegularExpression(QStringLiteral("/\\*"));
           CommentEnd  = QRegularExpression(QStringLiteral("\\*/"));

}

void Highlighter::highlightBlock(const QString &text)
{
    for (const HighlightingRule &rule : qAsConst(HighlightingRules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
    setCurrentBlockState(0);
    int startIndex = 0;
    if (previousBlockState() != 1)
           startIndex = text.indexOf(CommentStart);

    while (startIndex >= 0) {
          QRegularExpressionMatch match = CommentEnd.match(text, startIndex);
          int endIndex = match.capturedStart();
          int commentLength = 0;
          if (endIndex == -1) {
              setCurrentBlockState(1);
              commentLength = text.length() - startIndex;
          } else {
              commentLength = endIndex - startIndex
                              + match.capturedLength();
          }
          setFormat(startIndex, commentLength, multiLineCommentFormat);
          startIndex = text.indexOf(CommentStart, startIndex + commentLength);
      }
}

