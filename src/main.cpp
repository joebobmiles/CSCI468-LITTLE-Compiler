#include <antlr4-runtime.h>
#include <TINYLexer.h>
#include <TINYParser.h>
#include <TINYVisitor.h>
#include <TINYListener.h>
#include <TINYBaseVisitor.h>
#include <TINYBaseListener.h>

#include <fstream>
#include <string>

using namespace antlr4;
using namespace std;

class OurListener : public TINYBaseListener {
public:
  //virtual void enterFile(TINYParser::FileContext *ctx) override {
  //    printf("file: %s\n\n", ctx->getText().c_str());
  //}


  virtual void enterEveryRule(antlr4::ParserRuleContext *ctx) override {
      if (ctx->exception) {
          throw ctx->exception;
      }
      //printf("%s\n", ctx->getText().c_str());
  }
  //virtual void exitEveryRule(antlr4::ParserRuleContext *ctx) override { }

  //virtual void visitTerminal(antlr4::tree::TerminalNode *node) override {
  //    printf("   %s\n", node->toString().c_str());
  //}
  //virtual void visitErrorNode(antlr4::tree::ErrorNode *node) override { }
};

void bufferString(char *buffer, size_t size, char *string) {
    char *end = buffer + size - 1;

    do {
        *buffer = *string;
    }
    while (*string++ && buffer++ < end);

    *end = 0; /* force null terminator */
}

#define TOKENS_FILE "antlr/TINY.tokens"
char *typeToString(size_t typeId) {
    static char buffer[0x100]; /* static so that we can return it */
    int found = 0;
    size_t foundTypeId;

    /* There wasn't an obvious way to do this using the Antlr4 API, so we
     * decided to look it up out of the generated .tokens file. It's a bit
     * wasteful to open the file every time, but it would take a bit of effort
     * to, say, generate a lookup table that wasn't just as wasteful with
     * space. It could be done, but this will suffice for now. */

    FILE *file = fopen(TOKENS_FILE, "r");

    if (file) {
        for (;;) {
            int ret = fscanf(file, "%[^=]=%lu\n", buffer, &foundTypeId);
            found = (typeId == foundTypeId);

            if (found || ret == -1) break;
        }

        if (!found) {
            bufferString(buffer, sizeof buffer, "UNKNOWN");
        }

        fclose(file);
    }
    else {
        bufferString(buffer, sizeof buffer, "<FILE " TOKENS_FILE " NOT FOUND>");
    }

    return buffer;
}

int main(int argc, char **argv) {
    ifstream file(argc == 1? "example.tiny": argv[1]);

    if (file.is_open()) {
        ANTLRInputStream input(file);
        TINYLexer lexer(&input);
        CommonTokenStream tokenStream(&lexer);
        TINYParser parser(&tokenStream);

        try {
            OurListener listener;
            tree::ParseTree *tree = parser.file();
            tree::ParseTreeWalker::DEFAULT.walk(&listener, tree);
            printf("Accepted\n");
        }
        catch (const exception_ptr e) {
            printf("Not accepted\n");
        }

        file.close();
    }
}
