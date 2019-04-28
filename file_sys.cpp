// $Id: file_sys.cpp,v 1.6 2018-06-27 14:44:57-07 - - $

#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include "commands.h"
#include "debug.h"
#include <iostream>
#include <iomanip>

using namespace std;

#include "debug.h"
#include "file_sys.h"

int inode::next_inode_nr {1};

struct file_type_hash {
   size_t operator() (file_type type) const {
      return static_cast<size_t> (type);
   }
};

ostream& operator<< (ostream& out, file_type type) {
   static unordered_map<file_type,string,file_type_hash> hash {
      {file_type::PLAIN_TYPE, "PLAIN_TYPE"},
      {file_type::DIRECTORY_TYPE, "DIRECTORY_TYPE"},
   };
   return out << hash[type];
}

inode_ptr directory::mk_root_dir() {
    inode_ptr dir(new inode(file_type::DIRECTORY_TYPE));

    auto nd = dynamic_cast<directory*>(dir.get()->get_contents().get());
    nd->dirents["."] = dir;
    nd->dirents[".."] = dir;
    nd->name = "root";

    return dir;
}
void inode_state::set_cwd(inode_ptr ptr) {

    this->cwd = ptr;
}

inode_state::inode_state() {
    root = directory::mk_root_dir();
    cwd = root;

    DEBUGF ('i', "root = " << root << ", cwd = " << cwd
    << ", prompt = \"" << prompt() << "\"");
}
const inode_ptr& inode_state::get_cwd() const {
    return this->cwd;
}
const inode_ptr& inode_state::get_root() const {
    return this->root;
}

const string& inode_state::prompt() const { return prompt_; }

ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

void inode_state::set_prompt(const string & newPrompt) {
    this->prompt_ = newPrompt;
}

inode::inode(file_type type): inode_nr (next_inode_nr++) {
   switch (type) {
      case file_type::PLAIN_TYPE:
           contents = make_shared<plain_file>();
           break;
      case file_type::DIRECTORY_TYPE:
           contents = make_shared<directory>();
           break;
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

base_file_ptr inode::get_contents() const {
    return contents;
}

int inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}


file_error::file_error (const string& what):
            runtime_error (what) {
}

size_t plain_file::size() const {
    uint i = 0;
    for (string s: this->data){
    i += s.size();
    }
    i += this->data.size()-1;
    return size_t {i};
}

const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

void plain_file::writefile (const wordvec& words) {
   DEBUGF ('i', words);
    for (uint i = 2; i < words.size() ; ++i) {
        data.push_back(words[i]);
    }

}

const wordvec& plain_file::get_data() {
    return this->data;
}

void plain_file::remove (const string&) {
   throw file_error ("is a plain file");
}

inode_ptr plain_file::mkdir (inode_ptr parent, const string&) {
   throw file_error ("is a plain file");
}

inode_ptr plain_file::mkfile (const string&) {
   throw file_error ("is a plain file");

}


size_t directory::size() const {
   return this->dirents.size();
}

 map<string,inode_ptr>& directory::get_dirents()  {
    return this->dirents;
}

const wordvec& directory::readfile() const {
   throw file_error ("is a directory");
}

void directory::writefile (const wordvec&) {
   throw file_error ("is a directory");
}

void directory::remove (const string& filename) {
   DEBUGF ('i', filename);
}

inode_ptr directory::mkdir (inode_ptr parent, const string& dirname) {
   DEBUGF ('i', dirname);

    inode_ptr dir(new inode(file_type::DIRECTORY_TYPE));
    this->dirents.insert(pair<string, inode_ptr>(dirname, dir));

    auto nd = dynamic_cast<directory*>(dir.get()->get_contents().get());
    nd->dirents["."] = dir;
    nd->dirents[".."] = parent;
    nd->name = dirname;

    return dir;
}
const string directory::get_name() {
    return this->name;
}

void directory::remove(wordvec& pathname, inode_state& state) {
    auto cwinode = state.get_cwd();
    auto content = cwinode.get()->get_contents();
    auto dir = dynamic_cast<directory *>(content.get());
    auto dirmap = dir->get_dirents();
    string s = pathname.at(1);
    try {
        auto cnt = dirmap[s].get()->get_contents();

        if (dynamic_cast<directory *>(cnt.get()) != 0){
            auto del = dynamic_cast<directory *>(cnt.get());
            //dir
            if (del->size() > 2){
                throw command_error(": dir not empty");
            }

        }
        dirmap.erase(s);
    }
    catch (exception& e){
        throw command_error(pathname.at(0) + ": not found");
    }
}

const inode_ptr directory::search(wordvec pathname, inode_state& state) {
    auto cwinode = state.get_cwd();
    auto searchnode = state.get_cwd();
    string target = pathname.back();
    pathname.pop_back();
    for (int i = 0; i < pathname.size(); ++i) {
        auto content = searchnode.get()->get_contents();
        auto dir = dynamic_cast<directory *>(content.get());
        auto dirmap = dir->get_dirents();
        try {
            searchnode = dirmap.at(pathname.at(i));
        } catch (exception& e){
            //not found
        }
    }
    auto searchdir = dynamic_cast<directory *>(searchnode.get()->get_contents().get());
    try{
        return searchdir->get_dirents().at(target);
    } catch (exception& e){

    }

}

inode_ptr directory::mkfile (const string& filename) {
   DEBUGF ('i', filename);
    inode_ptr file(new inode(file_type::PLAIN_TYPE));
    this->dirents[filename] = file;
    return file;
}

