// $Id: commands.cpp,v 1.17 2018-01-25 14:02:55-08 - - $

#include "commands.h"
#include "debug.h"
#include <iostream>
#include <iomanip>

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
    auto cwinode = state.get_cwd();
    auto content = cwinode.get()->get_contents();
    auto dir = dynamic_cast<directory *>(content.get());
    if (words.size() > 2){
        throw exception{};
    }
    auto pathname = split(words.at(1),"/");
    try {
        auto dest = dir->search(pathname,state);
        auto file = dynamic_cast<plain_file *>(dest.get()->get_contents().get());
        if (file == nullptr){
            cout << "not a file" << endl;
            return;
        }
        auto data = file->get_data();
        for (uint i = 0; i < data.size(); ++i) {
            cout << data.at(i);
            if (i <data.size() - 1){
                cout <<" ";
            }
        }
        cout << endl;
    } catch (out_of_range& e){
        cout << "file not found" << endl;
    }
}

void fn_cd(inode_state &state, const wordvec &words) {
    DEBUGF ('c', state);
    DEBUGF ('c', words);


    if ((words.size() == 1) or (((words.size()) == 2) and (words.at(1) == "/"))) {
        state.set_cwd(state.get_root());
        return;
    }

    if (words.size() > 2) {
        //error
    } else if (words.size() == 2) {
        auto pathname =  split(words.at(1),"/");
        auto cwinode = state.get_cwd();
        auto content = cwinode.get()->get_contents();
        auto dir = dynamic_cast<directory *>(content.get());
        try {
            auto ncwd = dir->search(pathname,state);
            state.set_cwd(ncwd);
        } catch (exception &e) {
            cout << e.what();
        }
    }
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

        } catch (std::invalid_argument &) {
            exit_status::set(127);
        }
    } else {
        throw command_error(words[0] + ": invalid number of arguments");
    }
    throw ysh_exit();
}

void fn_ls(inode_state &state, const wordvec &words) {
    DEBUGF ('c', state);
    DEBUGF ('c', words);
    auto cwinode = state.get_cwd();
    auto content = cwinode.get()->get_contents();
    auto dir = dynamic_cast<directory *>(content.get());
    if (words.size() > 1) {

    } else {
        auto dirents = dir->get_dirents();
        if (cwinode.get()->get_inode_nr() == 1) {
            cout << "/:" << endl;
        } else {
            auto rootnode = state.get_root();
            auto currnode = state.get_cwd();
            auto cnt = content;
            auto currDir = dir;
            wordvec v;
            while (rootnode.get()->get_inode_nr() != currnode.get()->get_inode_nr()) {
                cnt = currnode.get()->get_contents();
                currDir = dynamic_cast<directory *>(cnt.get());
                currnode = currDir->get_dirents().at("..");
                v.push_back(currDir->get_name());
            }

            for (int i = v.size() - 1; i >= 0; --i) {
                cout << "/" << v.at(i) ;
            }
            cout <<  ":" << endl;
        }
        for (auto it = dirents.begin(); it != dirents.end(); ++it) {
            auto item = *it;
            cout << right << setw(6) << item.second.get()->get_inode_nr() << "  " <<
                 setw(6) << item.second.get()->get_contents().get()->size() << "  " <<
                 left << item.first;

            auto dr = dynamic_cast<directory *>(item.second.get()->get_contents().get());
            if (dr != 0){
                if (item.first != ".." and item.first != "."){
                    cout << "/";
                }
            }
                cout << endl;
        }
    }
}

void fn_lsr(inode_state &state, const wordvec &words) {
    DEBUGF ('c', state);
    DEBUGF ('c', words);
}

void fn_make(inode_state &state, const wordvec &words) {
    DEBUGF ('c', state);
    DEBUGF ('c', words);
    auto cwinode = state.get_cwd();
    auto content = cwinode.get()->get_contents();
    auto newfile = content.get()->mkfile(words.at(1));
    newfile.get()->get_contents().get()->writefile(words);

}

void fn_mkdir(inode_state &state, const wordvec &words) {
    DEBUGF ('c', state);
    DEBUGF ('c', words);
    auto cwinode = state.get_cwd();
    auto content = cwinode.get()->get_contents();
    auto newdir = content.get()->mkdir(cwinode, words.at(1));
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
    auto cwinode = state.get_cwd();
    auto content = cwinode.get()->get_contents();
    auto dir = dynamic_cast<directory *>(content.get());
    if (words.size() > 1) {

    } else {
        auto dirents = dir->get_dirents();
        if (cwinode.get()->get_inode_nr() == 1) {
            cout << "/" << endl;
        } else {
            auto rootnode = state.get_root();
            auto currnode = state.get_cwd();
            auto cnt = content;
            auto currDir = dir;
            wordvec v;
            while (rootnode.get()->get_inode_nr() != currnode.get()->get_inode_nr()) {
                cnt = currnode.get()->get_contents();
                currDir = dynamic_cast<directory *>(cnt.get());
                currnode = currDir->get_dirents().at("..");
                v.push_back(currDir->get_name());
            }

            for (int i = v.size() - 1; i >= 0; --i) {
                cout << "/" << v.at(i) ;
            }
            cout << endl;
        }
    }
}

void fn_rm(inode_state &state, const wordvec &words) {
    DEBUGF ('c', state);
    DEBUGF ('c', words);
    auto cwinode = state.get_cwd();
    auto content = cwinode.get()->get_contents();
    auto dir = dynamic_cast<directory *>(content.get());
    auto dirmap = dir->get_dirents();
    string s = words.at(1);
    try {
        auto cnt = dirmap[s].get()->get_contents();

        if (dynamic_cast<directory *>(cnt.get()) != 0){
           auto del = dynamic_cast<directory *>(cnt.get());
            //dir
            if (del->size() > 2){
                throw command_error(words.at(0) + ": dir not empty");
            }

        }
        dirmap.erase(s);
    }
    catch (exception& e){
    throw command_error(words.at(0) + ": not found");
    }
}

void fn_rmr(inode_state &state, const wordvec &words) {
    DEBUGF ('c', state);
    DEBUGF ('c', words);
}

