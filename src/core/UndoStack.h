#pragma once
#include <memory>
#include <vector>
#include <string>

namespace ds::core {

// Every mutating edit (trim, move, effect param change, color grade tweak)
// should be expressed as a Command so undo/redo, autosave-diffing, and
// future collaboration/version-history all share one mechanism.
class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual std::string description() const = 0;
};

class UndoStack {
public:
    void push(std::unique_ptr<Command> cmd) {
        cmd->execute();
        undoStack_.push_back(std::move(cmd));
        redoStack_.clear();
    }

    bool undo() {
        if (undoStack_.empty()) return false;
        auto cmd = std::move(undoStack_.back());
        undoStack_.pop_back();
        cmd->undo();
        redoStack_.push_back(std::move(cmd));
        return true;
    }

    bool redo() {
        if (redoStack_.empty()) return false;
        auto cmd = std::move(redoStack_.back());
        redoStack_.pop_back();
        cmd->execute();
        undoStack_.push_back(std::move(cmd));
        return true;
    }

    bool canUndo() const { return !undoStack_.empty(); }
    bool canRedo() const { return !redoStack_.empty(); }

private:
    std::vector<std::unique_ptr<Command>> undoStack_;
    std::vector<std::unique_ptr<Command>> redoStack_;
};

} // namespace ds::core
