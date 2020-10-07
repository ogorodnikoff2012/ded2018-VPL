#pragma once

#include <string>
#include <stdexcept>
#include <sstream>
#include <vector>
#include <unordered_map>

#define LOG out_ << '#' << __PRETTY_FUNCTION__ << '\n';

namespace vpl {

class Function {
public:
    explicit Function(const std::string& name) : name_(name) {}

    const std::vector<std::string>& GetArgs() const {
        return args_;
    }

    void AddArgument(const std::string& name) {
        args_.push_back(name);
    }

    const std::string& GetName() const {
        return name_;
    }

    bool AreEquivalent(const Function& other) const {
        return name_ == other.name_ && args_.size() == other.args_.size();
    }

private:
    std::vector<std::string> args_;
    std::string name_;
};

class NameScope {
public:
    void AddVariable(const std::string& name) {
        if (Contains(name)) {
            std::stringstream ss;
            ss << "Variable `" << name << "` already exists";
            throw std::runtime_error(ss.str());
        }
        offsets_[name] = total_size_;
        ++total_size_;
    }

    int Size() const {
        return total_size_;
    }

    bool IsGlobal() const {
        return is_global_;
    }

    void SetGlobal(bool value) {
        is_global_ = value;
    }

    bool Contains(const std::string& name) const {
        return offsets_.find(name) != offsets_.end();
    }

    int GetOffset(const std::string& name) const {
        return offsets_.find(name)->second;
    }

    const std::unordered_map<std::string, int> GetOffsets() const {
        return offsets_;
    }

private:
    int total_size_ = 0;
    std::unordered_map<std::string, int> offsets_;
    bool is_global_ = false;
};

class Scope {
public:
    explicit Scope(std::ostream& out) : out_(out), scopes_(1) {
        scopes_.back().SetGlobal(true);
    }

    void Finish() {
        LOG
        for (size_t i = 0; i < scopes_.size(); ++i) {
            if (scopes_[i].IsGlobal()) {
                for (const auto& [name, offset] : scopes_[i].GetOffsets()) {
                    out_ << "    var " << name << " 1\n";
                }
            }
        }
    }

    int GetUnique() {
        return ++unique_id_;
    }

    int GetPreviousUnique() const {
        return unique_id_;
    }

    const std::string& GetLastFunctionName() const {
        return functions_.back().GetName();
    }

    void SpawnFunctionScope() {
        OpenScope();
        for (const auto& arg : functions_.back().GetArgs()) {
            scopes_.back().AddVariable(arg);
        }
    }

    void OpenScope() {
        scopes_.emplace_back();
    }

    void CloseScope() {
        scopes_.pop_back();
    }

    void SaveStackPos() {
        stack_pos_hist_.push_back(cur_stack_pos_);
    }

    void RestoreStackPos() {
        cur_stack_pos_ = stack_pos_hist_.back();
        stack_pos_hist_.pop_back();
    }

    int RetRegister() const {
        return 3;
    }

    int AssignRegister() const {
        return 2;
    }

    int StackPointerRegister() const {
        return 1;
    }

    void CreateVariable(const std::string& name) {
        scopes_.back().AddVariable(name);
    }

    void DeclareFunction(const std::string& name) {
        functions_.emplace_back(name);
    }

    void EndFunctionDeclaration() {
        const auto& name = functions_.back().GetName();
        auto iter = func_name_map_.find(name);
        if (iter == func_name_map_.end()) {
            func_name_map_[name] = functions_.size() - 1;
            return;
        }
        if (!functions_.back().AreEquivalent(functions_[iter->second])) {
            std::stringstream ss;
            ss << "Function `" << name << "` already has another signature";
            throw std::runtime_error(ss.str());
        }
    }

    void AddArgumentName(const std::string& name) {
        functions_.back().AddArgument(name);
    }

    void LoadVariableAddr(const std::string& name) {
        LOG
        for (auto iter = scopes_.rbegin(); iter != scopes_.rend(); ++iter) {
            if (iter->Contains(name)) {
                if (!iter->IsGlobal()) {
                    out_ << "    push " << iter->GetOffset(name) << '\n';
                    out_ << "    push %" << StackPointerRegister() << '\n';
                    out_ << "    add\n";
                } else {
                    out_ << "    push " << name << '\n';
                }
                ++cur_stack_pos_;
                return;
            }
        }
        std::stringstream ss;
        ss << "Undefined variable `" << name << '`';
        throw std::runtime_error(ss.str());
    }

    void LoadVariable(const std::string& name) {
        LOG
        LoadVariableAddr(name);
        out_ << "    pop %" << AssignRegister() << '\n';
        out_ << "    push !" << AssignRegister() << '\n';
    }

    void PrepareFunction(const std::string& name) {
        LOG
        auto iter = func_name_map_.find(name);
        if (iter == func_name_map_.end()) {
            std::stringstream ss;
            ss << "Undefined function `" << name << '`';
            throw std::runtime_error(ss.str());
        }
        cur_func_ptr_ = &functions_[iter->second];
        out_ << "    push %" << StackPointerRegister() << '\n';
        Push();
    }

    void CheckArgCnt(size_t arg_cnt) {
        if (cur_func_ptr_ == nullptr) {
            throw std::runtime_error("Function is not prepared!");
        }
        if (arg_cnt != cur_func_ptr_->GetArgs().size()) {
            std::stringstream ss;
            ss << "Wrong number of arguments: expected " << cur_func_ptr_->GetArgs().size() << ", got " << arg_cnt;
            throw std::runtime_error(ss.str());
        }
    }

    void CallFunction(const std::string& name) {
        LOG
        Function* func = &functions_[func_name_map_[name]];

        int frame_size = 0;
        for (size_t i = 0; i < scopes_.size(); ++i) {
            if (!scopes_[i].IsGlobal()) {
                frame_size += scopes_[i].Size();
            }
        }

        out_ << "    push " << frame_size << '\n';
        out_ << "    push %" << StackPointerRegister() << '\n';
        out_ << "    add\n";
        out_ << "    pop %" << StackPointerRegister() << '\n';

        for (size_t i = 0; i < func->GetArgs().size(); ++i) {
            out_ << "    pop !" << StackPointerRegister() << '\n';
            out_ << "    push %" << StackPointerRegister() << '\n';
            out_ << "    push 1\n";
            out_ << "    add\n";
            out_ << "    pop %" << StackPointerRegister() << '\n';
            Pop();
        }
        out_ << "    push %" << StackPointerRegister() << '\n';
        out_ << "    push " << func->GetArgs().size() << "\n";
        out_ << "    sub\n";
        out_ << "    pop %" << StackPointerRegister() << '\n';

        out_ << "    call " << name << '\n';
        out_ << "    pop %" << StackPointerRegister() << '\n';
        out_ << "    push %" << RetRegister() << '\n';
    }

    void Push() {
        ++cur_stack_pos_;
    }

    void Pop() {
        --cur_stack_pos_;
    }

private:
    int unique_id_ = 0;
    std::vector<Function> functions_;
    std::vector<NameScope> scopes_;
    int cur_stack_pos_ = 0;
    std::vector<int> stack_pos_hist_;
    Function* cur_func_ptr_ = nullptr;
    std::unordered_map<std::string, int> func_name_map_;
    std::ostream& out_;
};

}  /* namespace vpl */
