#include <vector>
#include <memory>
#include <string>
#include <regex>
#include <iostream>

class Logger {
public:
    explicit Logger(const char* name) : name_(name) {
        Indent();
        ++depth_;
        std::cerr << "ENTER: " << name << std::endl;
    }
    ~Logger() {
        --depth_;
        Indent();
        if (std::uncaught_exceptions() > 0) {
            std::cerr << "FAIL: ";
        } else {
            std::cerr << "SUCCESS: ";
        }
        std::cerr << name_ << std::endl;
    }
private:
    void Indent() const {
        for (int i = 0; i < depth_; ++i) {
            std::cerr << ' ';
        }
    }

    const char* name_;
    static int depth_;
};

// TODO REMOVE NAFIG
int Logger::depth_ = 0;

#define DEFGRAMMAR(name) namespace name##Grammar {




#define DEFBASICNODE()                                                                              \
class ASTNodeBasic {                                                                                \
public:                                                                                             \
    using NodePtr = std::unique_ptr<ASTNodeBasic>;                                                  \
                                                                                                    \
    void Tx() { txs_.push_back(children_.size()); }                                                 \
                                                                                                    \
    void Rollback() { children_.resize(txs_.back()); txs_.pop_back(); }                             \
                                                                                                    \
    void Commit() {txs_.pop_back(); }                                                               \
                                                                                                    \
    auto& GetChildren() { return children_; }                                                       \
                                                                                                    \
    void Append(NodePtr&& node) {                                                                   \
        children_.emplace_back(std::move(node));                                                    \
    }                                                                                               \
                                                                                                    \
    virtual ~ASTNodeBasic() = default;                                                              \
                                                                                                    \
    virtual int GetType() const = 0;                                                                \
                                                                                                    \
    virtual const char* GetName() const = 0;                                                        \
                                                                                                    \
protected:                                                                                          \
    std::vector<NodePtr> children_;                                                                 \
    std::vector<std::size_t> txs_;                                                                  \




#define ENDBASICNODE()                                                                              \
};                                                                                                  \
                                                                                                    \
class SyntaxError : public std::runtime_error {                                                     \
public:                                                                                             \
    explicit SyntaxError(const std::string& s) : std::runtime_error(s) {}                           \
};                                                                                                  \
                                                                                                    \
class Parser {                                                                                      \
    using NodePtr = std::unique_ptr<ASTNodeBasic>;




#define ENDGRAMMAR(name)                                                                            \
    private:                                                                                        \
        const std::string* str_;                                                                    \
        std::string::const_iterator pos_;                                                           \
};                                                                                                  \
} /* namespace */




#define DEFRULE(name)                                                                               \
private:                                                                                            \
    static constexpr int k##name##Type = __COUNTER__;                                               \
                                                                                                    \
public:                                                                                             \
    class name##Node : public ASTNodeBasic {                                                        \
    public:                                                                                         \
        virtual int GetType() const override { return k##name##Type; }                              \
        virtual const char* GetName() const override { return #name ; }




#define BEGINRULE(name)                                                                             \
    };                                                                                              \
private:                                                                                            \
    NodePtr Parse##name() {                                                                         \
        Logger logger(__PRETTY_FUNCTION__);                                                         \
        NodePtr result = std::make_unique< name##Node >();




#define ENDRULE(name)                                                                               \
        return result;                                                                              \
    }




#define DEFRULES()
#define ENDRULES()




#define EXPECT(name, body) { NodePtr child = body; result->Append(std::move(child)); }




#define TX result->Tx(); auto old_pos = pos_;
#define COMMIT result->Commit();
#define ROLLBACK result->Rollback(); pos_ = old_pos;




#define ASTERISK(body)                                                                              \
while (true) {                                                                                      \
    TX;                                                                                             \
    try {                                                                                           \
        body;                                                                                       \
        COMMIT;                                                                                     \
    } catch (SyntaxError&) {                                                                        \
        ROLLBACK;                                                                                   \
        break;                                                                                      \
    }                                                                                               \
}




#define OR(left, right)                                                                             \
{                                                                                                   \
    TX;                                                                                             \
    try {                                                                                           \
        left;                                                                                       \
        COMMIT;                                                                                     \
    } catch (SyntaxError&) {                                                                        \
        ROLLBACK;                                                                                   \
        right;                                                                                      \
    }                                                                                               \
}




#define OR3(a, b, c) OR(a, OR(b, c))
#define OR4(a, b, c, d) OR(a, OR3(b, c, d))
#define OR5(a, b, c, d, e) OR(a, OR4(b, c, d, e))
#define OR6(a, b, c, d, e, f) OR(a, OR5(b, c, d, e, f))
#define MAYBE(body) OR(body, )

#define DEFTOKENS()
#define ENDTOKENS()




#define DEFTOKEN(name, regexp)                                                                      \
    private:                                                                                        \
        std::regex Regex##name {"(" regexp ")[^]*"};                                                  \
        static constexpr int k##name##Type = __COUNTER__;                                           \
                                                                                                    \
    NodePtr NextToken##name() {                                                                     \
        Logger logger(__PRETTY_FUNCTION__);                                                         \
        /* Skip spaces */                                                                           \
        while (pos_ != str_->end() && std::isspace(*pos_)) { ++pos_; }                              \
        std::smatch match;                                                                          \
        bool success = std::regex_match(pos_, str_->cend(), match, Regex##name );                   \
        if (!success) {                                                                             \
            std::string what = #name ": Bad token at pos ";                                         \
            what += std::to_string(pos_ - str_->cbegin());                                          \
            throw SyntaxError(what);                                                                \
        }                                                                                           \
                                                                                                    \
        auto submatch = match[1];                                                                   \
        pos_ += match.position(1) + submatch.length();                                              \
        return std::make_unique<name##Node>(submatch.str());                                        \
    }                                                                                               \
                                                                                                    \
    public:                                                                                         \
        class name##Node : public ASTNodeBasic {                                                    \
        public:                                                                                     \
            explicit name##Node(const std::string& str) : str_(str) {}                              \
                                                                                                    \
            const std::string& GetStr() const { return str_; }                                      \
                                                                                                    \
            virtual int GetType() const override { return k##name##Type; }                          \
                                                                                                    \
            virtual const char* GetName() const override { return #name ; }                         \
                                                                                                    \
        private:                                                                                    \
            std::string str_;                                                                       \
        public:




#define ENDTOKEN() };




#define MAINRULE(name)                                                                              \
    public:                                                                                         \
        NodePtr Parse(const std::string& str) {                                                     \
            str_ = &str;                                                                            \
            pos_ = str.begin();                                                                     \
            return Parse##name();                                                                   \
        }




#define RULE(name) Parse##name()
#define TOKEN(name) NextToken##name()
