// $Id: commands.cpp,v 1.17 2018-01-25 14:02:55-08 - - $

#include "commands.h"
#include "debug.h"

command_hash cmd_hash{
        {"cat",    fn_cat},
        {"cd",     fn_cd},
        {"echo",   fn_echo},
        {"exit",   fn_exit},
        {"ls",     fn_ls},
        {"lsr",    fn_lsr},
        {"make",   fn_make},
        {"mkdir",  fn_mkdir},
        {"prompt", fn_prompt},
        {"pwd",    fn_pwd},
        {"rm",     fn_rm},
};

command_fn find_command_fn(const string &cmd) {
    // Note: value_type is pair<const key_type, mapped_type>
    // So: iterator->first is key_type (string)
    // So: iterator->second is mapped_type (command_fn)

    DEBUGF ('c', "[" << cmd << "]");
    const auto result = cmd_hash.find(cmd);
    if (result == cmd_hash.end()) {
        throw command_error(cmd + ": no such function");
    }
    return result->second;
}

command_error::command_error(const string &what) :
        runtime_error(what) {
}

int exit_status_message() {
    int exit_status = exit_status::get();
    cout << execname() << ": exit(" << exit_status << ")" << endl;
    return exit_status;
}

void fn_cat(inode_state &state, const wordvec &words) {
    DEBUGF ('c', state);
    DEBUGF ('c', words);
}

void fn_cd(inode_state &state, const wordvec &words) {
    DEBUGF ('c', state);
    DEBUGF ('c', words);
}

void fn_echo(inode_state &state, const wordvec &words) {
    DEBUGF ('c', state);
    DEBUGF ('c', words);
    cout << word_range(words.cbegin() + 1, words.cend()) << endl;
}


void fn_exit(inode_state &state, const wordvec &words) {
    DEBUGF ('c', state);
    DEBUGF ('c', words);

    if (words.size() == 1) {
        exit_status::set(0);
    } else if (words.size() == 2) {
        try {
            int i = std::stoi(words[1]);
            exit_status::set(i);

        } catch (std::invalid_argument) {
            exit_status::set(127);
        }
    } else{
        throw command_error(words[0] + ": invalid number of arguments");
    }
    throw ysh_exit();
}

void fn_ls(inode_state &state, const wordvec &words) {
    DEBUGF ('c', state);
    DEBUGF ('c', words);
}

void fn_lsr(inode_state &state, const wordvec &words) {
    DEBUGF ('c', state);
    DEBUGF ('c', words);
}

void fn_make(inode_state &state, const wordvec &words) {
    DEBUGF ('c', state);
    DEBUGF ('c', words);
}

void fn_mkdir(inode_state &state, const wordvec &words) {
    DEBUGF ('c', state);
    DEBUGF ('c', words);
}

void fn_prompt(inode_state &state, const wordvec &words) {
    DEBUGF ('c', state);
    DEBUGF ('c', words);
    string s;
    for (uint i = 1; i < words.size(); ++i) {
        s += words[i] + " ";
    }
    state.set_prompt(s);
}

void fn_pwd(inode_state &state, const wordvec &words) {
    DEBUGF ('c', state);
    DEBUGF ('c', words);
}

void fn_rm(inode_state &state, const wordvec &words) {
    DEBUGF ('c', state);
    DEBUGF ('c', words);
}

void fn_rmr(inode_state &state, const wordvec &words) {
    DEBUGF ('c', state);
    DEBUGF ('c', words);
}

