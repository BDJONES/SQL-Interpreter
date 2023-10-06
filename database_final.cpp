#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <tuple>
#include <unordered_map>
#include <algorithm>
#include <list>
#include <ctype.h>

using namespace std;

class tab_columns_parser {
    private:
    tab_columns_parser* tc_parser;    
    string tab_name;
    string col_name;
    string col_type;
    int col_id;
    public:
    tab_columns_parser() {
        this -> tc_parser = nullptr;
        this -> tab_name = "";
        this -> col_name = "";
        this -> col_type = "";
        this -> col_id = 0;
    }
    tab_columns_parser(string tname, string cname, string ctype, int id) {
        this -> tab_name = tname;
        this -> col_name = cname;
        this -> col_type = ctype;
        this -> col_id = id;
    }
    string getTabName() {
        return this -> tab_name;
    }
    void setTabName(string table_name) {
        this -> tab_name = table_name;
    }
    string getColName() {
        return this -> col_name;
    }
    void setColName(string column_name) {
        this -> col_name = column_name;
    }
    string getColType() {
        return this -> col_type;
    }
    void setColType(string column_type) {
        this -> col_type = column_type;
    }
    int getColId() {
        return this -> col_id;
    }
    void setColId(string column_id) {
        this -> col_id = stoi(column_id);
    }

};

struct less_than_id {
    inline bool operator() (tab_columns_parser p1, tab_columns_parser p2) {
        return (p1.getColId() < p2.getColId());
    }
};

struct sort_index_cmp {
    inline bool operator() (string p1, string p2) {
        return (p1 < p2);
    }
};

class Select {
    private:
    Select* select;
    vector<string> select_conditions;
    unordered_map<string,string> columns_list;
    bool list_type;
    public:
    Select() {
        this -> select = nullptr;
        this -> list_type = false;
        this -> select_conditions = vector<string>();
        this -> columns_list = unordered_map<string,string>();
        // this -> select_conditions = vector<string>();
    }
    vector<string> getSelectCons() {
        return this -> select_conditions;
    }
    void parse_select(string select_statement) {
        select_statement.erase(remove(select_statement.begin(), select_statement.end(), ' '));
        stringstream s_stream(select_statement);
        while(s_stream.good()) {
            string substring;
            //cout << "here" << endl;
            getline(s_stream, substring, ',');
            this->select_conditions.push_back(substring);
            //cout << substring << endl;
        }
        for (int i = 0; i < this -> select_conditions.size(); i++) {
            stringstream dict_stream(this->select_conditions.at(i));
            string col_name;
            string col_val;
            int count = 0;
            while (dict_stream.good()) {
                if (count == 0) {
                    getline(dict_stream, col_name, ':');
                }
                else {
                    getline(dict_stream, col_val);
                    this -> columns_list.insert({col_name, col_val});
                    if (col_val == "1") {
                        this -> list_type = true;
                    }
                    else {
                        this -> list_type = false;
                    }
                }
                count++;
            }
        }
        //sort(columns_list.begin(), columns_list.end());
    }
    vector<int> getIndexList(unordered_map<string, vector<tab_columns_parser>> tab_cols_dict, bool list_type, string tab_name) {
        vector<int> all_indecies;
        if (list_type) {
            for (auto i : this -> columns_list) {
                string col_name =  i.first;
                for (int j = 0; j < tab_cols_dict.at(tab_name).size(); j++) {
                    if (col_name == tab_cols_dict.at(tab_name).at(j).getColName()) {
                        all_indecies.emplace(all_indecies.begin(),j);
                    }
                }
            }
        }
        else if (!list_type && this -> select_conditions.at(0) != "*") {
            for (auto i : this -> columns_list) {
                string col_name =  i.first;
                for (int j = 0; j < tab_cols_dict.at(tab_name).size(); j++) {
                    if (tab_cols_dict.at(tab_name).at(j).getColName() == col_name || this -> columns_list.find(tab_cols_dict.at(tab_name).at(j).getColName()) != this -> columns_list.end()) {
                        continue;
                    }
                    if (count(all_indecies.begin(), all_indecies.end(), j) == 0) {
                        all_indecies.push_back(j);
                    }
                }
            }
        }
        else {
            // cout << "for star" << endl;
            for (int i = 0; i < tab_cols_dict.at(tab_name).size(); i++) {
                all_indecies.push_back(i);
            }
        }
        //sort(all_indecies.begin(), all_indecies.end());
        return all_indecies;
    }
    bool getListType() {
        return this -> list_type;
    }
    unordered_map<string,string> getInExList() {
        return this -> columns_list;
    }
};

class From {
    private:
    From* from;
    string table_name;
    public:
    From() {
        this -> from = nullptr;
        this -> table_name = "";
    }
    string getTableName() {
        return this -> table_name;
    }
    void parse_from(string from_statement) {
        from_statement.erase(remove(from_statement.begin(), from_statement.end(), ' '));      
        this -> table_name = from_statement;
    }
};

class Where {
    private:
    // split each where clause into 3 vectors to contain each part of the where clause
    Where* where;
    vector<string> compare_cols;
    vector<string> compare_ops;
    vector<string> compare_vals;
    public:
    Where() {
        this -> where = nullptr;
        this -> compare_cols = vector<string>();
        this -> compare_ops = vector<string>();
        this -> compare_vals = vector<string>();
    }
    vector<string> getCompareCols() {
        return this -> compare_cols;
    }
    vector<string> getCompareOps() {
        return this -> compare_ops;
    }
    vector<string> getCompareVals() {
        return this -> compare_vals;
    }
    void parse_where(string where_clause) { // BASE ON WHETHER SOMETHING IS A S[ECIAL CHARACTER OR NOT AND THEN ADD COMMAS
        //cout << where_clause << endl;
        where_clause.erase(remove(where_clause.begin(), where_clause.end(), ' '), where_clause.end());
        //cout << where_clause << endl;
        for(int i = 0; i < where_clause.length(); i++) {
            //cout << where_clause.at(i) << endl;
            if (where_clause.at(i) == ' ') {
                where_clause.at(i) = ',';
            }
        }
        //cout << where_clause << endl;
        stringstream s_stream(where_clause);
        int count = 1;
        while(s_stream.good()) {
            string token;
            getline(s_stream, token, ',');
            //token.erase(find(token.begin(), token.end(), ' '), token.end());
            if (count == 1) {
                string col_name = "";
                for (int i = 0; i < token.length(); i++) {
                    if (isalpha(token.at(i))) {
                        col_name += token.at(i);
                    }
                    else {
                        string temp(token.begin() + i, token.end());
                        token = temp;
                        break;
                    }
                }
                compare_cols.push_back(col_name);
                count++;
            }
            if (count == 2) {
                string col_name = "";
                bool first_punct = false;
                bool second_punct = false;
                if (ispunct(token.at(0)) && ispunct(token.at(1))) {
                    for (int i = 0; i < token.length(); i++) {
                        if (ispunct(token.at(i)) && !first_punct) {
                            col_name += token.at(i);
                            first_punct = true;
                            continue;
                        }
                        else if (ispunct(token.at(i)) && first_punct) {
                            col_name += token.at(i);
                            string temp(token.begin() + i + 1, token.end());
                            token = temp;
                            //cout << "The token: " << token << endl;
                            break;
                        }
                    }
                    //cout << col_name << endl;
                    compare_ops.push_back(col_name);
                    count++;
                }
                else {
                    col_name += token.at(0);
                    string temp(token.begin() + 1, token.end());
                    token = temp;
                    compare_ops.push_back(col_name);
                    count++;
                }
            }
            if (count == 3) {
                compare_vals.push_back(token);
                count = 0;
            }
            count++;
        }
        // cout << "compare_cols" << endl;
        // for (int i = 0; i < compare_cols.size(); i++) {
        //     cout << compare_cols.at(i) << endl;
        // }
        // for (int i = 0; i < compare_ops.size(); i++) {
        //     cout << compare_ops.at(i) << endl;
        // }
        // for (int i = 0; i < compare_vals.size(); i++) {
        //     cout << compare_vals.at(i) << endl;
        // }
    }
    vector<int> getCompIndecies(vector<tab_columns_parser> tab_col_info) {
        vector<int> comp_ind_list;
        for (int i = 0; i < this -> compare_cols.size(); i++) {
            for (int j = 0; j < tab_col_info.size(); j++) {
                if (compare_cols.at(i) == tab_col_info.at(j).getColName()) {
                    comp_ind_list.push_back(tab_col_info.at(j).getColId() - 1);
                }
            }
        }
        // for_each(comp_ind_list.begin(), comp_ind_list.end(), [](int num) {
        //     cout << num << endl;
        // });
        return comp_ind_list;
    }
    bool compare_vals_func(vector<tab_columns_parser> tab_col_info, int index, string col_val) {
        int tab_ind = 0;
        string col_type;
        for (int i = 0; i < compare_cols.size(); i++) {
            if (tab_col_info.at(index).getColName() == this -> compare_cols.at(i)) {
                tab_ind = i;
                col_type = tab_col_info.at(index).getColType();
            }         
        }
        if (this -> compare_ops.at(tab_ind) == ">") {
            if (col_type == "STRING") {
                if (find(col_val.begin(),col_val.end(),' ') != col_val.end()) {
                    col_val.erase(find(col_val.begin(), col_val.end(), ' '), col_val.end());
                }
                return col_val > this -> compare_vals.at(tab_ind); 
            }
            else if (col_type == "INT" && col_val == " ") {
                return 0 > stoi(this -> compare_vals.at(tab_ind));
            }
            else if (col_type == "INT" && col_val != " ") {
                cout << col_val.length() << endl;
                cout << col_val << endl;
                cout << this -> compare_vals.at(tab_ind) << endl;
                return stoi(col_val) > stoi(this -> compare_vals.at(tab_ind));
            }
            else if (col_type == "FLOAT" && col_val == " ") {
                return stof("0") > stof(this -> compare_vals.at(tab_ind));
            }
            else if (col_type == "FLOAT" && col_val != " ") {
                return stof(col_val) > stof(this -> compare_vals.at(tab_ind));
            }
            else {
                const char* str_to_char = this -> compare_vals.at(tab_ind).c_str();
                return col_val.at(0) > str_to_char[0];
            }
        }
        else if (this -> compare_ops.at(tab_ind) == "<") {
            if (col_type == "STRING") {
                if (find(col_val.begin(),col_val.end(),' ') != col_val.end()) {
                    col_val.erase(find(col_val.begin(), col_val.end(), ' '), col_val.end());
                }
                return col_val < this -> compare_vals.at(tab_ind); 
            }
            else if (col_type == "INT" && col_val == " ") {
                return 0 < stoi(this -> compare_vals.at(tab_ind));
            }
            else if (col_type == "INT" && col_val != " ") {
                return stoi(col_val) < stoi(this -> compare_vals.at(tab_ind));
            }
            else if (col_type == "FLOAT" && col_val == " ") {
                return stof("0") < stof(this -> compare_vals.at(tab_ind));
            }
            else if (col_type == "FLOAT" && col_val != " ") {
                return stof(col_val) < stof(this -> compare_vals.at(tab_ind));
            }
            else {
                const char* str_to_char = this -> compare_vals.at(tab_ind).c_str();
                return col_val.at(0) < str_to_char[0];
            }
        }
        else if (this -> compare_ops.at(tab_ind) == "=") {
            if (col_type == "STRING") {
                if (find(col_val.begin(),col_val.end(),' ') != col_val.end()) {
                    col_val.erase(find(col_val.begin(), col_val.end(), ' '), col_val.end());
                }
                return col_val == this -> compare_vals.at(tab_ind); 
            }
            else if (col_type == "INT" && col_val == " ") {
                return 0 == stoi(this -> compare_vals.at(tab_ind));
            }
            else if (col_type == "INT" && col_val != " ") {
                return stoi(col_val) == stoi(this -> compare_vals.at(tab_ind));
            }
            else if (col_type == "FLOAT" && col_val == " ") {
                return stof("0") == stof(this -> compare_vals.at(tab_ind));
            }
            else if (col_type == "FLOAT" && col_val != " ") {
                return stof(col_val) == stof(this -> compare_vals.at(tab_ind));
            }
            else {
                const char* str_to_char = this -> compare_vals.at(tab_ind).c_str();
                return col_val.at(0) == str_to_char[0];
            }
        }
        else if (this -> compare_ops.at(tab_ind) == ">=") {
            if (col_type == "STRING") {
                if (find(col_val.begin(),col_val.end(),' ') != col_val.end()) {
                    col_val.erase(find(col_val.begin(), col_val.end(), ' '), col_val.end());
                }
                return col_val >= this -> compare_vals.at(tab_ind); 
            }
            else if (col_type == "INT" && col_val == " ") {
                
                return 0 >= stoi(this -> compare_vals.at(tab_ind));
            }
            else if (col_type == "INT" && col_val != " ") {
                
                return stoi(col_val) >= stoi(this -> compare_vals.at(tab_ind));
            }
            else if (col_type == "FLOAT" && col_val == " ") {
                
                return stof("0") >= stof(this -> compare_vals.at(tab_ind));
            }
            else if (col_type == "FLOAT" && col_val != " ") {
                return stof(col_val) >= stof(this -> compare_vals.at(tab_ind));
            }
            else {
                const char* str_to_char = this -> compare_vals.at(tab_ind).c_str();
                return col_val.at(0) >= str_to_char[0];
            }
        }
        else if (this -> compare_ops.at(tab_ind) == "<=") {
            if (col_type == "STRING") {
                if (find(col_val.begin(),col_val.end(),' ') != col_val.end()) {
                    col_val.erase(find(col_val.begin(), col_val.end(), ' '), col_val.end());
                }
                return col_val <= this -> compare_vals.at(tab_ind); 
            }
            else if (col_type == "INT" && col_val == " ") {
                return 0 <= stoi(this -> compare_vals.at(tab_ind));
            }
            else if (col_type == "INT" && col_val != " ") {
                return stoi(col_val) <= stoi(this -> compare_vals.at(tab_ind));
            }
            else if (col_type == "FLOAT" && col_val == " ") {
                return stof("0") <= stof(this -> compare_vals.at(tab_ind));
            }
            else if (col_type == "FLOAT" && col_val != " ") {
                return stof(col_val) <= stof(this -> compare_vals.at(tab_ind));
            }
            else {
                const char* str_to_char = this -> compare_vals.at(tab_ind).c_str();
                return col_val.at(0) <= str_to_char[0];
            }
        }
        else {
            if (col_type == "STRING") {
                if (find(col_val.begin(),col_val.end(),' ') != col_val.end()) {
                    col_val.erase(find(col_val.begin(), col_val.end(), ' '), col_val.end());
                }
                return col_val != this -> compare_vals.at(tab_ind); 
            }
            else if (col_type == "INT" && col_val == " ") {
                return 0 != stoi(this -> compare_vals.at(tab_ind));
            }
            else if (col_type == "INT" && col_val != " ") {
                return stoi(col_val) != stoi(this -> compare_vals.at(tab_ind));
            }
            else if (col_type == "FLOAT" && col_val == " ") {
                return stof("0") != stof(this -> compare_vals.at(tab_ind));
            }
            else if (col_type == "FLOAT" && col_val != " ") {
                return stof(col_val) != stof(this -> compare_vals.at(tab_ind));
            }
            else {
                const char* str_to_char = this -> compare_vals.at(tab_ind).c_str();
                return col_val.at(0) != str_to_char[0];
            }
        }                    
    }
    
};

class Orderby {
    private:
    vector<string> sort_conditions;
    vector<int> sort_orderings;
    unordered_map<string,string> sorting_dict;
    public:
    Orderby() {
        this -> sort_conditions = vector<string>();
        this -> sort_orderings = vector<int>();
        this -> sorting_dict = unordered_map<string,string>();
    }
    vector<string> getSortCons() {
        return sort_conditions;
    }
    void parse_orderby(string orderby_clause) {
        //cout << "parsing orderby" << endl;
        orderby_clause.erase(remove(orderby_clause.begin(), orderby_clause.end(), ' '), orderby_clause.end());
        //cout << orderby_clause << endl;
        stringstream s_stream(orderby_clause);
        while(s_stream.good()) {
            string substring;
            //cout << "here" << endl;
            getline(s_stream, substring, ',');
            this->sort_conditions.push_back(substring);
            //cout << substring << endl;
        }
        for (int i = 0; i < this -> sort_conditions.size(); i++) {
            stringstream dict_stream(this->sort_conditions.at(i));
            string col_name;
            string col_val;
            int count = 0;
            while (dict_stream.good()) {
                if (count == 0) {
                    getline(dict_stream, col_name, ':');
                }
                else {
                    getline(dict_stream, col_val);
                    this -> sorting_dict.insert({col_name, col_val});
                }
                count++;
            }
        }
    }
    vector<vector<string>> sort_orderby(vector<tab_columns_parser> tab_col_info,vector<vector<string>> all_tab_data, vector<int> indecies) {
        for (auto i: sorting_dict) {
            for (int j = 0; j < indecies.size(); j++) {
                for (int k = 0; k < tab_col_info.size(); k++) {
                    if (i.first == tab_col_info.at(k).getColName() && indecies.at(j) == tab_col_info.at(k).getColId() - 1) {
                        string col_type = tab_col_info.at(k).getColType();
                        if (i.second == "1") {
                            sort(all_tab_data.begin(), all_tab_data.end(), [j, col_type](vector<string> a, vector<string> b) {
                                if (col_type == "STRING") {
                                    if (find(a.at(j).begin(),a.at(j).end(),' ') != a.at(j).end()) {
                                        a.at(j).erase(find(a.at(j).begin(), a.at(j).end(), ' '), a.at(j).end());
                                        //cout << "here" << endl;
                                    }
                                    if (find(b.at(j).begin(),b.at(j).end(),' ') != b.at(j).end()) {
                                        b.at(j).erase(find(b.at(j).begin(), b.at(j).end(), ' '), b.at(j).end());
                                    }
                                    return a.at(j) < b.at(j);
                                }
                                else if (col_type == "INT" && a.at(j) == " " && b.at(j) != " ") {
                                    return 0 < stoi(b.at(j));
                                }
                                else if (col_type == "INT" && a.at(j) != " " && b.at(j) == " ") {
                                    return stoi(a.at(j)) < 0;
                                }
                                else if (col_type == "INT" && a.at(j) == " " && b.at(j) == " ") {
                                    return 0 < 0;
                                }
                                else if (col_type == "FLOAT" && a.at(j) == " " && b.at(j) != " ") {
                                    float zero = stof("0");
                                    return zero < stof(b.at(j));
                                }
                                else if (col_type == "FLOAT" && a.at(j) != " " && b.at(j) == " ") {
                                    float zero = stof("0");
                                    return stof(a.at(j)) < zero;
                                }
                                else if (col_type == "FLOAT" && a.at(j) == " " && b.at(j) == " ") {
                                    float zero = stof("0");
                                    return zero < zero;
                                }
                                else if (col_type == "FLOAT" && a.at(j) != " " && b.at(j) != " ") {
                                    return stof(a.at(j)) < stof(a.at(j));
                                }
                                else {
                                    return a.at(j).at(0) < b.at(j).at(0);
                                }
                            }); 
                        }
                        else if (i.second == "-1") {
                            sort(all_tab_data.begin(), all_tab_data.end(), [j,col_type](vector<string> a, std::vector<string> b) {
                                //cout << "a.at(" << j << ") = " << a.at(j) << endl;
                                if (col_type == "STRING") {
                                    if (find(a.at(j).begin(),a.at(j).end(),' ') != a.at(j).end()) {
                                        a.at(j).erase(find(a.at(j).begin(), a.at(j).end(), ' '), a.at(j).end());
                                        //cout << "here" << endl;
                                    }
                                    if (find(b.at(j).begin(),b.at(j).end(),' ') != b.at(j).end()) {
                                        b.at(j).erase(find(b.at(j).begin(), b.at(j).end(), ' '), b.at(j).end());
                                    }
                                    return a.at(j) > b.at(j);
                                }
                                else if (col_type == "INT" && a.at(j) == " " && b.at(j) != " ") {
                                    return 0 > stoi(b.at(j));
                                }
                                else if (col_type == "INT" && a.at(j) != " " && b.at(j) == " ") {
                                    return stoi(a.at(j)) > 0;
                                }
                                else if (col_type == "INT" && a.at(j) == " " && b.at(j) == " ") {
                                    return 0 > 0;
                                }
                                else if (col_type == "FLOAT" && a.at(j) == " " && b.at(j) != " ") {
                                    float zero = stof("0");
                                    return zero > stof(b.at(j));
                                }
                                else if (col_type == "FLOAT" && a.at(j) != " " && b.at(j) == " ") {
                                    float zero = stof("0");
                                    return stof(a.at(j)) > zero;
                                }
                                else if (col_type == "FLOAT" && a.at(j) == " " && b.at(j) == " ") {
                                    float zero = stof("0");
                                    return zero > zero;
                                }
                                else if (col_type == "FLOAT" && a.at(j) != " " && b.at(j) != " ") {
                                    return stof(a.at(j)) > stof(a.at(j));
                                }
                                else {
                                    return a.at(j).at(0) > b.at(j).at(0);
                                }
                            }); 
                        }
                        
                    }
                }                
            }
        }
        return all_tab_data;
    }
};

class Query {
    private:
    Query* query;
    bool whr_pres;
    bool ord_pres;
    public:
    Select* select;
    From* from;
    Where* where;
    Orderby* orderby;
    Query() {
        this -> query = nullptr;
        this -> whr_pres = false;
        this -> ord_pres = false;
    }
    Query(Select* select_reqs, From* table_req) {
        this -> select = select_reqs;
        this -> from = table_req;
        this -> where = nullptr;
        this -> orderby = nullptr;
        whr_pres = false;
        ord_pres = false;
    }
    Query(Select* select_reqs, From* table_req, Where* where_reqs) {
        this -> select = select_reqs;
        this -> from = table_req;
        this -> where = where_reqs;
        this -> orderby = nullptr;
        whr_pres = true;
        ord_pres = false;
    }
    Query(Select* select_reqs, From* table_req, Orderby* order_reqs) {
        this -> select = select_reqs;
        this -> from = table_req;
        this -> where = nullptr;
        this -> orderby = order_reqs;
        this -> whr_pres = false;
        this -> ord_pres = true;
    }
    Query(Select* select_reqs, From* table_req, Where* where_reqs, Orderby* order_reqs) {
        this -> select = select_reqs;
        this -> from = table_req;
        this -> where = where_reqs;
        this -> orderby = order_reqs;
        this -> whr_pres = true;
        this -> ord_pres = true;
    }
    void runQuery(unordered_map<string,vector<tab_columns_parser>> table_data, int clear_lvl) {
        string table_file = from -> getTableName() + ".csv";
        string str_clear_lvl = to_string(clear_lvl);
        vector<int> table_indecies = select -> getIndexList(table_data, select -> getListType(), from -> getTableName());
        string sec_index = printTabColNames(table_data, table_indecies, from -> getTableName(), select -> getListType());
        vector<vector<string>> result;
        if (whr_pres) {
            vector<int> where_indecies = where -> getCompIndecies(table_data.at(from -> getTableName()));
            result = parseTableFile(table_data.at(from -> getTableName()), table_indecies, where_indecies, table_file, sec_index, str_clear_lvl, select -> getListType(), table_data.at(from -> getTableName()).size());
        }
        else {
            result = parseTableFile(table_data.at(from -> getTableName()), table_indecies, table_file, sec_index, str_clear_lvl, select -> getListType(), table_data.at(from -> getTableName()).size());
        }
        printResult(result, table_indecies, select -> getListType());
    }
    string printTabColNames(unordered_map<string,vector<tab_columns_parser>> table_data, vector<int> index_list, string tab_name, bool list_type) {
        string all_cols;
        vector<tab_columns_parser> curr_vector = table_data.at(tab_name);
        string security_index;
        int curr_size = 1;
        //cout << index_list.size() << endl;
        bool already_printed = false;
        //cout << table_data.at(tab_name).size() << endl;
        for (int i = 0; i < table_data.at(tab_name).size(); i++) {
            //cout << "here" << endl;
            if (table_data.at(tab_name).at(i).getColName() == "TC") {
                //cout << "Here" << endl;
                security_index = to_string(i);
                cout << security_index << endl;
            }
        }
        for (int i = 0; i < curr_vector.size(); i++) {

            // cout << curr_vector.at(i).getColName()
            if (i < curr_vector.size() - 1) {
                if (select -> getSelectCons().at(0) == "*") {
                    cout << curr_vector.at(i).getColName() << ",";
                }
                else if (select -> getListType() && !already_printed) {
                    if (select -> getInExList().find(curr_vector.at(i).getColName()) != select -> getInExList().end() && curr_size < select -> getInExList().size()) {
                        for (int j = 0; j < index_list.size(); j++) {
                            //cout << index_list.at(j) << endl;
                            if (j < index_list.size() - 1) {
                                for (int k = 0; k < table_data.size(); k++) {
                                    if (table_data.at(this -> from -> getTableName()).at(k).getColId() - 1 == index_list.at(j)) {
                                        //cout << index_list.at(j) << endl;
                                        cout << table_data.at(this -> from -> getTableName()).at(index_list.at(j)).getColName() << ",";
                                        break;
                                    }
                                }
                            } 
                            else {
                                for (int k = 0; k < table_data.size(); k++) {
                                    if (table_data.at(this -> from -> getTableName()).at(k).getColId() - 1 == j) {
                                        cout << table_data.at(this -> from -> getTableName()).at(index_list.at(j)).getColName() << endl;
                                        break;
                                    }
                                }
                                
                            }
                        }
                        already_printed = true;
                        //cout << curr_vector.at(i).getColName() << ",";
                        // curr_size++;
                    }
                    else if (select -> getInExList().find(curr_vector.at(i).getColName()) != select -> getInExList().end() && curr_size == select -> getInExList().size()) {
                        cout << curr_vector.at(i).getColName() << endl;
                        curr_size++;
                    }
                }
                else if (!select -> getListType()) {
                    if (select -> getInExList().find(curr_vector.at(i).getColName()) == select -> getInExList().end() && curr_size < (curr_vector.size() - select -> getInExList().size())) {
                        cout << curr_vector.at(i).getColName() << ",";
                        curr_size++;
                    }
                    else if (select -> getInExList().find(curr_vector.at(i).getColName()) == select -> getInExList().end() && curr_size == (curr_vector.size() - select -> getInExList().size())) {
                        cout << curr_vector.at(i).getColName() << endl;
                        curr_size++;                        
                    }
                }
            }
            else {
                if (select -> getSelectCons().at(0) == "*") {
                    cout << curr_vector.at(i).getColName() << endl;
                }
                else if (select -> getInExList().find(curr_vector.at(i).getColName()) != select -> getInExList().end() && curr_size == select -> getInExList().size()) {
                    cout << curr_vector.at(i).getColName() << endl;
                    curr_size++;
                }
                else if (select -> getInExList().find(curr_vector.at(i).getColName()) == select -> getInExList().end() && curr_size == (curr_vector.size() - select -> getInExList().size())) {
                    cout << curr_vector.at(i).getColName() << endl;
                    curr_size++;                        
                }
            }
        }
        return security_index;
    }
    vector<vector<string>> parseTableFile(vector<tab_columns_parser> table_data, vector<int> tab_indecies, vector<int> where_indecies, string fileName, string sec_ind, string sec_lev, bool list_type, int tab_size) {
        ifstream table_queried;
        string complete_row;
        vector<vector<string>> query_result;
        table_queried.open(fileName);
        vector<string> all_tab_rows;
        while (getline(table_queried,complete_row, '\n')) { // move this to anothe loop, but still get each line in this loop. This will let you delimit the values and see which ones should print
            all_tab_rows.push_back(complete_row);
        }
        table_queried.close();
        vector<vector<string>> tokenized_input;
        bool add_to_result_lvl = false;
        bool add_to_result_val_inserted = false;
        int col_num = 0;
        bool isStar = false;
       
        for(int i = 0; i < all_tab_rows.size(); i++) {
            string cur_vector = all_tab_rows.at(i);
            stringstream s_stream(cur_vector);
            vector<string> new_vec_inex(tab_indecies.size(),"");
            vector<string> new_vec_all;
            int count = 0;
            vector<int> sorted_list = tab_indecies;
            sort(sorted_list.begin(), sorted_list.end());
            bool comp_success = false;
            //int count = 0;
            while (s_stream.good()) {
                string token;
                getline(s_stream , token, ',');
                //cout << col_num << endl;
                //cout << "token = " << token << endl;
                int first_char = 0;
                for (int j = 0; j < token.length(); j++) {
                    if (isalpha(token[j]) || isdigit(token[j]) || token[j] == ',') {
                        first_char = j;
                        // cout << "first_char = " << first_char << endl;
                        break;
                    }
                }
                if (first_char != 0) {
                    token.erase(remove(token.begin(), token.begin() + first_char, ' '), token.begin() + first_char);
                }
                if (find(where_indecies.begin(), where_indecies.end(), col_num) != where_indecies.end() && this -> whr_pres) {
                    //cout << "here" << endl;
                    comp_success = this -> where -> compare_vals_func(table_data, col_num, token);
                    if (!comp_success) {
                        break;
                    }
                }
                if (list_type == true) {
                    if (find(tab_indecies.begin(), tab_indecies.end(), col_num) != tab_indecies.end()) {
                        auto find_index = find(tab_indecies.begin(), tab_indecies.end(), sorted_list.at(count));
                        int index = find_index - tab_indecies.begin();
                        new_vec_inex.at(index) = token;
                        count++;
                        add_to_result_val_inserted = true;
                        //cout << col_num << endl;
                    }
                }
                else if (list_type == false && this -> select -> getSelectCons().at(0) != "*") {
                    if (find(tab_indecies.begin(), tab_indecies.end(), col_num) != tab_indecies.end()) {
                        int index;
                        for (int j = 0; j < tab_indecies.size(); j++) {
                            if (col_num == tab_indecies.at(j)) {
                                index = j;
                            }
                        }
                        new_vec_inex.at(index) = token;
                        count++;
                        add_to_result_val_inserted = true;
                    }
                }
                else {
                    new_vec_all.push_back(token);
                    isStar = true;
                }
                //cout << "here" << endl;
                if (col_num == stoi(sec_ind)) {
                    if (stoi(sec_lev) >= stoi(token)) {
                        add_to_result_lvl = true;
                        col_num = 0;
                    }
                }
                col_num++;
            }
            if (!comp_success && whr_pres) {
                col_num = 0;
                continue;
            }
            if (isStar) {
              tokenized_input.push_back(new_vec_all);  
            }
            else {
                tokenized_input.push_back(new_vec_inex);
            }
            
            if (add_to_result_lvl && isStar) {
                query_result.push_back(new_vec_all);

            }
            else if (add_to_result_val_inserted && add_to_result_lvl && !isStar) {
                query_result.push_back(new_vec_inex);
                //cout << "here pushing to query result" << endl;
            }
            add_to_result_lvl = false;
            add_to_result_val_inserted = false;
            //cout << "ended loop = " << i << endl; 
            col_num = 0;
        }
        //cout << endl << "query_result = " << query_result.size() << endl;
        if (ord_pres) {
            query_result = orderby -> sort_orderby(table_data, query_result, tab_indecies); 
        }
        
        return query_result;
    }
    vector<vector<string>> parseTableFile(vector<tab_columns_parser> table_data, vector<int> tab_indecies, string fileName, string sec_ind, string sec_lev, bool list_type, int tab_size) {
        ifstream table_queried;
        string complete_row;
        vector<vector<string>> query_result;
        table_queried.open(fileName);
        vector<string> all_tab_rows;
        while (getline(table_queried,complete_row, '\n')) { // move this to anothe loop, but still get each line in this loop. This will let you delimit the values and see which ones should print
            all_tab_rows.push_back(complete_row);
        }
        table_queried.close();
        vector<vector<string>> tokenized_input;
        bool add_to_result_lvl = false;
        bool add_to_result_val_inserted = false;
        int col_num = 0;
        bool isStar = false;
       
        for(int i = 0; i < all_tab_rows.size(); i++) {
            string cur_vector = all_tab_rows.at(i);
            stringstream s_stream(cur_vector);
            vector<string> new_vec_inex(tab_indecies.size(),"");
            vector<string> new_vec_all;
            int count = 0;
            vector<int> sorted_list = tab_indecies;
            sort(sorted_list.begin(), sorted_list.end());
            //int count = 0;
            while (s_stream.good()) {
                string token;
                getline(s_stream , token, ',');
                //cout << col_num << endl;
                //cout << "token = " << token << endl;
                int first_char = 0;
                for (int j = 0; j < token.length(); j++) {
                    if (isalpha(token[j]) || isdigit(token[j])) {
                        first_char = j;
                        // cout << "first_char = " << first_char << endl;
                        break;
                    }
                }
                if (first_char != 0) {
                    token.erase(remove(token.begin(), token.begin() + first_char, ' '), token.begin() + first_char);
                }
                if (list_type == true) {
                    if (find(tab_indecies.begin(), tab_indecies.end(), col_num) != tab_indecies.end()) {
                        auto find_index = find(tab_indecies.begin(), tab_indecies.end(), sorted_list.at(count));
                        int index = find_index - tab_indecies.begin();
                        new_vec_inex.at(index) = token;
                        count++;
                        add_to_result_val_inserted = true;
                        //cout << col_num << endl;
                    }
                }
                else if (list_type == false && this -> select -> getSelectCons().at(0) != "*") {
                    if (find(tab_indecies.begin(), tab_indecies.end(), col_num) != tab_indecies.end()) {
                        int index;
                        for (int j = 0; j < tab_indecies.size(); j++) {
                            if (col_num == tab_indecies.at(j)) {
                                index = j;
                            }
                        }
                        new_vec_inex.at(index) = token;
                        count++;
                        add_to_result_val_inserted = true;
                    }
                }
                else {
                    new_vec_all.push_back(token);
                    isStar = true;
                }
                //cout << "here" << endl;
                if (col_num == stoi(sec_ind)) {
                    if (stoi(sec_lev) >= stoi(token)) {
                        add_to_result_lvl = true;
                        col_num = 0;
                    }
                }
                col_num++;
            }
            if (isStar) {
              tokenized_input.push_back(new_vec_all);  
            }
            else {
                tokenized_input.push_back(new_vec_inex);
            }
            
            if (add_to_result_lvl && isStar) {
                query_result.push_back(new_vec_all);

            }
            else if (add_to_result_val_inserted && add_to_result_lvl && !isStar) {
                query_result.push_back(new_vec_inex);
                //cout << "here pushing to query result" << endl;
            }
            add_to_result_lvl = false;
            add_to_result_val_inserted = false;
            //cout << "ended loop = " << i << endl; 
            col_num = 0;
        }
        //cout << endl << "query_result = " << query_result.size() << endl;
        if (ord_pres) {
           query_result = orderby -> sort_orderby(table_data, query_result, tab_indecies); 
        }
        
        return query_result;
    }
    void printResult (vector<vector<string>> q_res, vector<int> index_list, bool list_type) {
        for (int i = 0; i < q_res.size(); i++) {
            for (int j = 0; j < q_res.at(i).size(); j++) {
                if (j != q_res.at(i).size() - 1) {
                    cout << q_res.at(i).at(j) << ',';
                }
                else {
                    cout << q_res.at(i).at(j) << endl;
                }
            }
        }
    }
};

Query* parse_query(string query);

int main(int argc, char** argv) {
    if (argc != 2) {
        cout << "Incorrect number of arguments" << endl;
        return -1;
    }
    int sec_clearance = atoi(argv[1]);
    unordered_map<string,vector<tab_columns_parser>> all_table_info;
    vector<string> file_input; 
    ifstream myfile;
    myfile.open("TAB_COLUMNS.csv");
    string column_info;
    int count = 0;
    while (getline(myfile, column_info, '\n')) {
        file_input.push_back(column_info);
    }
    myfile.close();
    for(int i = 0; i < file_input.size(); i++) {
        tab_columns_parser all_column_data;
        string current_tuple = file_input.at(i);
        stringstream s_stream(current_tuple);
        while (s_stream.good()) {
            string token;
            getline(s_stream , token, ',');
            if (count <= 3) {
                if (count == 0) {
                    all_column_data.setTabName(token);
                }
                else if (count == 1) {
                    all_column_data.setColName(token);
                }
                else if (count == 2) {
                    all_column_data.setColType(token);
                }
                else {
                    all_column_data.setColId(token);
                }
                count++;
            }
            if (count == 4) {
                if (all_table_info.find(all_column_data.getTabName()) == all_table_info.end()) {
                    vector<tab_columns_parser> new_vector;
                    new_vector.push_back(all_column_data);
                    all_table_info.insert({all_column_data.getTabName(),new_vector});
                }
                else {
                    all_table_info.at(all_column_data.getTabName()).push_back(all_column_data);
                }
                count = 0;
            }
        }
        sort(all_table_info.at(all_column_data.getTabName()).begin(), all_table_info.at(all_column_data.getTabName()).end(), less_than_id());
    }
    string query_input;
    while (1) {
        cout << "MLS> ";
        getline(cin,query_input,'\n');
        if (query_input == "EXIT") {
            break;
        }
        Query* query = parse_query(query_input);
        query -> runQuery(all_table_info, sec_clearance);
        free(query);
    }
    
    return 0;
}

Query* parse_query(string query) {
    vector<string> select_vector;
    Select* select = new Select();
    From* from = new From();
    Where* where = new Where();
    Orderby* orderby = new Orderby();
    query.erase(remove(query.begin(), query.end(), '\r'), query.end());
    //cout << query << endl;
    // stringstream s_stream(query);
    bool ord_pres = false;
    bool whr_pres = false;
    bool frm_pres = false;
    bool sel_pres = false;
    vector<char> cur_word;
    vector<string> separated_query;
    query.erase(query.end() - 1, query.end()); // removes semicolon
    for (int i = 0; i < query.length(); i++) {
        cur_word.push_back(query.at(i));
        string str(cur_word.begin(), cur_word.end());
        if (i == query.length() - 1) {
            //cout << str << endl;
            separated_query.push_back(str);
        }
        if (str == "SELECT") {
            cur_word.clear();
            sel_pres = true;
        }
        if (str.find(" FROM") != std::string::npos) {
            str.erase(str.end() - 5, str.end());
            //cout << str << endl;
            cur_word.clear();
            separated_query.push_back(str);
            frm_pres = true;
        }
        if (str.find(" WHERE") != std::string::npos) {
            str.erase(str.end() - 6, str.end());
            //cout << str << endl;
            cur_word.clear();
            separated_query.push_back(str);
            whr_pres = true;
        }
        if (str.find(" ORDERBY") != std::string::npos) {
            str.erase(str.end() - 8, str.end());
            //cout << str << endl;
            cur_word.clear();
            separated_query.push_back(str);
            ord_pres = true;
        }
    }
    bool whr_first = false;
    bool ord_first = false;
    for (int i = 0; i < separated_query.size(); i++) {
        string token = separated_query.at(i);
        if (i == 0) {
            select->parse_select(token);
        }
        if (i == 1) {
            from -> parse_from(token);
        }
        if (i == 2 && whr_pres) {
            where ->parse_where(token);
            whr_first = true;
            continue;
        }
        if (i == 2 && ord_pres) {
            orderby -> parse_orderby(token);
            ord_first == true;
            continue;
        }
        if (i == 3 && whr_first) {
            orderby -> parse_orderby(token);
        }
        else if (i == 3 && ord_first) {
            where -> parse_where(token);
        }
        //cout << "here" << endl;
    }
    Query* new_query;
    if (!whr_pres && !ord_pres) {
        new_query = new Query(select, from);    
    }
    else if (whr_pres && !ord_pres) {
        new_query = new Query(select, from, where);
    }
    else if (!whr_pres && ord_pres) {
        new_query = new Query(select, from, orderby);
    }
    else {
        new_query = new Query(select, from, where, orderby);
    }
    
    return new_query;
}

bool cmp_vect_index(vector<string> v1, vector<string> v2, int index) {
    if (v1.at(index) == v2.at(index)) {
        return v1 < v2;
    }
    if (v1.at(index) < v2.at(index)) {
        return v1 < v2;
    }
}