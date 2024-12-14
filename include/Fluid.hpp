#pragma once

#include "FixedInner.hpp"
#include "ParticleParams.hpp"
#include "Matrix.hpp"
#include "VectorField.hpp"
#include "Const.hpp"

#include <cstring>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <memory>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <csignal>

using namespace std;


template<typename PType, typename VType>
class ParticleParams;

template<typename PType, typename VType, typename VFType>
class Fluid {
    private:
        static constexpr size_t max_ticks = 1'000'000;
        using V_COMMON_TYPE = typename CommonTypeFixed<VType, VFType>::type;

        size_t  n, m;
        int     UT = 0;
        VType   g;
        PType   rho[256];
        size_t  start_tick = 0;

        unique_ptr<AbstractMatrix<PType>>   p       = nullptr;
        unique_ptr<AbstractMatrix<PType>>   old_p   = nullptr;
        VectorField<VType>                  velocity;
        VectorField<VFType>                 velocity_flow;

        unique_ptr<AbstractMatrix<char>>    field   = nullptr;
        unique_ptr<AbstractMatrix<int>>     last_use= nullptr;
        unique_ptr<AbstractMatrix<int>>     dirs    = nullptr;

        bool need_save;
        string save_filename;
        // bool save_signal;


    public:
        Fluid(const string& filename, bool need_save, const string& save_filename);

        void run(); 

    private:
        tuple<V_COMMON_TYPE, bool, pair<int, int>> propagate_flow(int x, int y, V_COMMON_TYPE lim);

        void propagate_stop(int x, int y, bool force = false);

        VType move_prob(int x, int y);

        bool propagate_move(int x, int y, bool is_first);

    private:
        // void saveSignalOn(int _);
        void save_to_file(size_t tick);
        
        void save_field(const AbstractMatrix<char>& field, ofstream& file);
        void save_velocity(const AbstractMatrix<array<VType, deltas.size()>>& velocity, ofstream& file);
        void save_last_use(const AbstractMatrix<int>& last_use, ofstream& file);
        void save_p(const AbstractMatrix<PType>& p, ofstream& file);
        void save_RHO(const PType rho[256], ofstream& file);

        void read_velocity(ifstream& file);
        void read_last_use(ifstream& file);
        void read_p(ifstream& file);


        bool read_line(ifstream& fin, string& line);

        void read_NM(ifstream &fin);

        void read_field(ifstream &fin);

        void read_G(ifstream &fin);

        void read_RHO(ifstream &fin);

        void read_default_file(ifstream& fin);
        void read_save_file(ifstream& fin);
        void read_main_data_from_savefile(ifstream& fin);
        
    friend ParticleParams<PType, VType>;
};










template<typename PType, typename VType, typename VFType>
void Fluid<PType, VType, VFType>::save_field(const AbstractMatrix<char>& field, ofstream& file) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            file << (field[i][j]);
        }
        file << "\n";
    }
}

template<typename PType, typename VType, typename VFType>
void Fluid<PType, VType, VFType>::save_velocity(const AbstractMatrix<array<VType, deltas.size()>>& velocity, ofstream& file) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            file << double(velocity[i][j][0]) << " " << double(velocity[i][j][1]) << " " << double(velocity[i][j][2]) << " "
                 << double(velocity[i][j][3]) << " ";
        }
        file << "\n";
    }
}

template<typename PType, typename VType, typename VFType>
void Fluid<PType, VType, VFType>::save_RHO(const PType rho[256], ofstream& file){
    for(int i = 0; i < 256; ++i){
        if (double(rho[i]) != 0){
            string ans = "";
            ans.push_back(char(i));
            ans += " " + to_string(rho[i]) + "\n";
            file << ans;
        }
    }
}

template<typename PType, typename VType, typename VFType>
void Fluid<PType, VType, VFType>::save_last_use(const AbstractMatrix<int>& last_use, ofstream& file) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            file << (last_use[i][j]) << " ";
        }
        file << "\n";
    }
}

template<typename PType, typename VType, typename VFType>
void Fluid<PType, VType, VFType>::save_p(const AbstractMatrix<PType>& p, ofstream& file) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            file << (double(p[i][j])) << " ";
        }
        file << "\n";
    }
}

template<typename PType, typename VType, typename VFType>
void Fluid<PType, VType, VFType>::read_velocity(ifstream& fin){
    string line;
    stringstream ss;

    for (size_t i = 0; i < n; ++i) {
        if (!read_line(fin, line)) {
            throw runtime_error("Не удалось прочитать параметр velocity");
        }
        for (size_t j = 0; j < m; ++j) {
            ss = stringstream{line};
            double temp[4];
            ss >> temp[0] >> temp[1] >> temp[2] >> temp[3];

            (*velocity.v)[i][j][0] = VType(temp[0]);
            (*velocity.v)[i][j][1] = VType(temp[1]);
            (*velocity.v)[i][j][2] = VType(temp[2]);
            (*velocity.v)[i][j][3] = VType(temp[3]);
        }
    }
}


template<typename PType, typename VType, typename VFType>
void Fluid<PType, VType, VFType>::read_last_use(ifstream& fin){
    string line;
    stringstream ss;
    for (size_t i = 0; i < n; ++i) {
        if (!read_line(fin, line)) {
            throw runtime_error("Не удалось прочитать параметр last_use");
        }
        for (size_t j = 0; j < m; ++j) {
            ss = stringstream{line};
            int temp;
            ss >> temp;
            (*last_use)[i][j] = temp;
        }
    }
}

template<typename PType, typename VType, typename VFType>
void Fluid<PType, VType, VFType>::read_p(ifstream& fin){
    string line;
    stringstream ss;
    for (size_t i = 0; i < n; ++i) {
        if (!read_line(fin, line)) {
            throw runtime_error("Не удалось прочитать параметр P");
        }

        for (size_t j = 0; j < m; ++j) {
            ss = stringstream{line};
            double temp;
            ss >> temp;
            (*p)[i][j] = PType(temp);
        }
    }
}

template<typename PType, typename VType, typename VFType>
void Fluid<PType, VType, VFType>::read_main_data_from_savefile(ifstream& fin){
    stringstream ss;
    string line;

    if (!read_line(fin, line)) {
        throw runtime_error("Не удалось прочитать главные параметры из файла сохранения");
    }
    ss = stringstream{line};
    if (!(ss >> n) || !(ss >> m) || !(ss >> start_tick) || !(ss >> UT) || !(ss >> g)) {
        throw runtime_error("Не удалось прочитать главные параметры из файла сохранения");
    }
}

template<typename PType, typename VType, typename VFType>
void Fluid<PType, VType, VFType>::save_to_file(size_t tick){
    ofstream file(save_filename);
    if (!file.is_open()) {
        cout << "Error: can`t open file `" << save_filename << "`" << endl;
    }
    file << 1 << "\n";
    file << n << " " << m << " " << tick << " " << UT << " " << double(g) << "\n";
    save_field(*field, file);
    save_velocity(*(velocity.v), file);
    save_last_use(*last_use, file);
    save_p(*p, file); 
    save_RHO(rho, file);
}


template<typename PType, typename VType, typename VFType>
void Fluid<PType, VType, VFType>::read_save_file(ifstream& fin){
    read_main_data_from_savefile(fin);

    field.reset(create_matrix<char>{}(n, m + 1));
    p.reset(create_matrix<PType>{}(n, m));
    old_p.reset(create_matrix<PType>{}(n, m));
    last_use.reset(create_matrix<int>{}(n, m));
    dirs.reset(create_matrix<int>{}(n, m));

    velocity = VectorField<VType>{n, m};
    velocity_flow = VectorField<VFType>{n, m};

    read_field(fin);
    read_velocity(fin);
    read_last_use(fin);
    read_p(fin);
    read_RHO(fin);
}


template<typename PType, typename VType, typename VFType>
void Fluid<PType, VType, VFType>::read_default_file(ifstream& fin){
    read_NM(fin);

    field.reset(create_matrix<char>{}(n, m + 1));
    p.reset(create_matrix<PType>{}(n, m));
    old_p.reset(create_matrix<PType>{}(n, m));
    last_use.reset(create_matrix<int>{}(n, m));
    dirs.reset(create_matrix<int>{}(n, m));
    
    velocity = VectorField<VType>{n, m};
    velocity_flow = VectorField<VFType>{n, m};
    
    read_field(fin);
    
    read_G(fin);

    read_RHO(fin);
}


template<typename PType, typename VType, typename VFType>
Fluid<PType, VType, VFType>::Fluid(const string& filename, bool need_save, const string& save_filename) {
    ifstream fin(filename);
    if (!fin) {
        throw runtime_error("Не удалось открыть файл");
    }

    int save_or_default_file;
    stringstream ss;
    string line;

    if (!read_line(fin, line)) {
        throw runtime_error("Не удалось прочитать");
    }
    ss = stringstream{line};
    if (!(ss >> save_or_default_file)) {
        throw runtime_error("Не удалось прочитать параметры N и M");
    }

    if(save_or_default_file == 0){read_default_file(fin);}
    else{read_save_file(fin);}

    this->need_save = need_save;
    this->save_filename = save_filename;
}

bool save_signal;


void saveSignalOn(int _) {
    save_signal = true;
}

template<typename PType, typename VType, typename VFType>
void Fluid<PType, VType, VFType>::run(){
    if(need_save){
        signal(SIGINT, saveSignalOn);
    }
    save_signal = false;

    for (size_t x = 0; x < n; ++x) {
        for (size_t y = 0; y < m; ++y) {
            if ((*field)[x][y] == '#')
                continue;
            for (auto [dx, dy] : deltas) {
                (*dirs)[x][y] += ((*field)[x + dx][y + dy] != '#');
            }
        }
    }

    for (size_t i = start_tick; i < max_ticks; ++i) {
        if(save_signal){
            save_to_file(i);
            cout << "SAAAAAVED" << endl;
            save_signal = false;
            cout << "Enter 0 to finish simulation and other key to continue: ";
            string ans;
            cin >> ans;
            if(ans == "0") return;    
        }


        PType total_delta_p = 0;
        // Apply external forces
        for (size_t x = 0; x < n; ++x) {
            for (size_t y = 0; y < m; ++y) {
                if ((*field)[x][y] == '#')
                    continue;
                if ((*field)[x + 1][y] != '#')
                    velocity.add(x, y, 1, 0, g);
            }
        }

        // Apply forces from p
        *old_p = *p;
        for (size_t x = 0; x < n; ++x) {
            for (size_t y = 0; y < m; ++y) {
                if ((*field)[x][y] == '#')
                    continue;
                for (auto [dx, dy] : deltas) {
                    int nx = x + dx, ny = y + dy;
                    if ((*field)[nx][ny] != '#' && (*old_p)[nx][ny] < (*old_p)[x][y]) {
                        auto delta_p = (*old_p)[x][y] - (*old_p)[nx][ny];
                        auto force = delta_p;
                        auto &contr = velocity.get(nx, ny, -dx, -dy);
                        if (contr * rho[(int) ((*field)[nx][ny])] >= force) {
                            contr -= force / rho[(int) ((*field)[nx][ny])];
                            continue;
                        }
                        force -= contr * rho[(int) ((*field)[nx][ny])];
                        contr = 0;
                        velocity.add(x, y, dx, dy, force / rho[(int) ((*field)[x][y])]);
                        (*p)[x][y] -= force / (*dirs)[x][y];
                        total_delta_p -= force / (*dirs)[x][y];
                    }
                }
            }
        }

        // Make flow from velocities
        velocity_flow.reset();
        bool prop = false;
        do {
            UT += 2;
            prop = 0;
            for (size_t x = 0; x < n; ++x) {
                for (size_t y = 0; y < m; ++y) {
                    if ((*field)[x][y] != '#' && (*last_use)[x][y] != UT) {
                        auto [t, local_prop, _] = propagate_flow(x, y, 1);
                        if (t > 0) {
                            prop = 1;
                        }
                    }
                }
            }
        } while (prop);

        // Recalculate p with kinetic energy
        for (size_t x = 0; x < n; ++x) {
            for (size_t y = 0; y < m; ++y) {
                if ((*field)[x][y] == '#')
                    continue;
                for (auto [dx, dy] : deltas) {
                    auto old_v = velocity.get(x, y, dx, dy);
                    auto new_v = velocity_flow.get(x, y, dx, dy);
                    if (old_v > 0) {
                        assert(new_v <= old_v);
                        velocity.get(x, y, dx, dy) = new_v;
                        auto force = (old_v - new_v) * rho[(int) ((*field)[x][y])];
                        if ((*field)[x][y] == '.')
                            force *= 0.8;
                        if ((*field)[x + dx][y + dy] == '#') {
                            (*p)[x][y] += force / (*dirs)[x][y];
                            total_delta_p += force / (*dirs)[x][y];
                        } else {
                            (*p)[x + dx][y + dy] += force / (*dirs)[x + dx][y + dy];
                            total_delta_p += force / (*dirs)[x + dx][y + dy];
                        }
                    }
                }
            }
        }

        UT += 2;
        prop = false;
        for (size_t x = 0; x < n; ++x) {
            for (size_t y = 0; y < m; ++y) {
                if ((*field)[x][y] != '#' && (*last_use)[x][y] != UT) {
                    if (Rnd::random01<double>() < move_prob(x, y)) {
                        prop = true;
                        propagate_move(x, y, true);
                    } else {
                        propagate_stop(x, y, true);
                    }
                }
            }
        }

        if (prop) {
            cout << "Tick " << i << ":\n";
            for (size_t x = 0; x < n; ++x) {
                cout << (*field)[x] << endl;
            }
        }
    }
}



template<typename PType, typename VType, typename VFType>
tuple<typename CommonTypeFixed<VType, VFType>::type, bool, pair<int, int>> Fluid<PType, VType, VFType>::propagate_flow(int x, int y, typename CommonTypeFixed<VType, VFType>::type lim) {
    (*last_use)[x][y] = UT - 1;
    V_COMMON_TYPE ret = 0;
    for (auto [dx, dy] : deltas) {
        int nx = x + dx, ny = y + dy;
        if ((*field)[nx][ny] != '#' && (*last_use)[nx][ny] < UT) {
            auto cap = velocity.get(x, y, dx, dy);
            auto flow = velocity_flow.get(x, y, dx, dy);
            if (flow == cap) {
                continue;
            }
            // assert(v >= velocity_flow.get(x, y, dx, dy));
            auto vp = min(lim, cap - flow);
            if ((*last_use)[nx][ny] == UT - 1) {
                velocity_flow.add(x, y, dx, dy, vp);
                (*last_use)[x][y] = UT;
                // cerr << x << " " << y << " -> " << nx << " " << ny << " " << vp << " / " << lim << "\n";
                return {vp, 1, {nx, ny}};
            }
            auto [t, prop, end] = propagate_flow(nx, ny, vp);
            ret += t;
            if (prop) {
                velocity_flow.add(x, y, dx, dy, t);
                (*last_use)[x][y] = UT;
                // cerr << x << " " << y << " -> " << nx << " " << ny << " " << t << " / " << lim << "\n";
                return {t, prop && end != make_pair(x, y), end};
            }
        }
    }
    (*last_use)[x][y] = UT;
    return {ret, 0, {0, 0}};
}


template<typename PType, typename VType, typename VFType>
void Fluid<PType, VType, VFType>::propagate_stop(int x, int y, bool force) {
    if (!force) {
        bool stop = true;
        for (auto [dx, dy] : deltas) {
            int nx = x + dx, ny = y + dy;
            if ((*field)[nx][ny] != '#' && (*last_use)[nx][ny] < UT - 1 && velocity.get(x, y, dx, dy) > 0) {
                stop = false;
                break;
            }
        }
        if (!stop) {
            return;
        }
    }
    (*last_use)[x][y] = UT;
    for (auto [dx, dy] : deltas) {
        int nx = x + dx, ny = y + dy;
        if ((*field)[nx][ny] == '#' || (*last_use)[nx][ny] == UT || velocity.get(x, y, dx, dy) > 0) {
            continue;
        }
        propagate_stop(nx, ny);
    }
}

template<typename PType, typename VType, typename VFType>
VType Fluid<PType, VType, VFType>::move_prob(int x, int y) {
    VType sum = 0;
    for (size_t i = 0; i < deltas.size(); ++i) {
        auto [dx, dy] = deltas[i];
        int nx = x + dx, ny = y + dy;
        if ((*field)[nx][ny] == '#' || (*last_use)[nx][ny] == UT) {
            continue;
        }
        auto v = velocity.get(x, y, dx, dy);
        if (v < 0) {
            continue;
        }
        sum += v;
    }
    return sum;
}

template<typename PType, typename VType, typename VFType>
bool Fluid<PType, VType, VFType>::propagate_move(int x, int y, bool is_first) {
    (*last_use)[x][y] = UT - is_first;
    bool ret = false;
    int nx = -1, ny = -1;
    do {
        array<VType, deltas.size()> tres;
        VType sum = 0;
        for (size_t i = 0; i < deltas.size(); ++i) {
            auto [dx, dy] = deltas[i];
            int nx = x + dx, ny = y + dy;
            if ((*field)[nx][ny] == '#' || (*last_use)[nx][ny] == UT) {
                tres[i] = sum;
                continue;
            }
            auto v = velocity.get(x, y, dx, dy);
            if (v < 0) {
                tres[i] = sum;
                continue;
            }
            sum += v;
            tres[i] = sum;
        }

        if (sum == 0) {
            break;
        }

        VType p = Rnd::random01<VType>() * sum;
        size_t d = ranges::upper_bound(tres, p) - tres.begin();

        auto [dx, dy] = deltas[d];
        nx = x + dx;
        ny = y + dy;
        assert(velocity.get(x, y, dx, dy) > 0 && (*field)[nx][ny] != '#' && (*last_use)[nx][ny] < UT);

        ret = ((*last_use)[nx][ny] == UT - 1 || propagate_move(nx, ny, false));
    } while (!ret);
    (*last_use)[x][y] = UT;
    for (size_t i = 0; i < deltas.size(); ++i) {
        auto [dx, dy] = deltas[i];
        int nx = x + dx, ny = y + dy;
        if ((*field)[nx][ny] != '#' && (*last_use)[nx][ny] < UT - 1 && velocity.get(x, y, dx, dy) < 0) {
            propagate_stop(nx, ny);
        }
    }
    if (ret) {
        if (!is_first) {
            ParticleParams<PType, VType> pp{};
            pp.swap_with(*this, x, y);
            pp.swap_with(*this, nx, ny);
            pp.swap_with(*this, x, y);
        }
    }
    return ret;
}

template<typename PType, typename VType, typename VFType>
void Fluid<PType, VType, VFType>::read_RHO(ifstream &fin){
    stringstream ss;
    string line;
    
    while (read_line(fin, line)) {
        if (line.find_first_not_of(' ') == string::npos || line.empty()) {
            break;
        }

        ss = stringstream{line};
        char c = ss.get();
        if (!(ss >> rho[c])) {
            throw runtime_error("Не удалось прочитать параметр RHO");
        }
    }
}

template<typename PType, typename VType, typename VFType>
void Fluid<PType, VType, VFType>::read_G(ifstream &fin){
    stringstream ss;
    string line;
    
    if (!read_line(fin, line)) {
        throw runtime_error("Не удалось прочитать параметр G");
    }
    ss = stringstream{line};
    if (!(ss >> g)) {
        throw runtime_error("Не удалось прочитать параметр G");
    }
}



template<typename PType, typename VType, typename VFType>
void Fluid<PType, VType, VFType>::read_field(ifstream &fin){
    string line;
    for (size_t i = 0; i < n; ++i) {
        if (!read_line(fin, line)) {
            throw runtime_error("Не удалось прочитать параметр Field");
        }
        if (line.size() != m) {
            throw runtime_error("Неверно указаны размеры поля");
        }
        for (size_t j = 0; j < m; ++j) {
            (*field)[i][j] = line[j];
        }
    }
}

template<typename PType, typename VType, typename VFType>
void Fluid<PType, VType, VFType>::read_NM(ifstream &fin){
    stringstream ss;
    string line;

    if (!read_line(fin, line)) {
        throw runtime_error("Не удалось прочитать");
    }
    ss = stringstream{line};
    if (!(ss >> n) || !(ss >> m)) {
        throw runtime_error("Не удалось прочитать параметры N и M");
    }
}

template<typename PType, typename VType, typename VFType>
bool Fluid<PType, VType, VFType>::read_line(ifstream& fin, string& line){
    while (true) {
        if (!getline(fin, line))    {return false;}
        if (line.starts_with("//")) {continue;}
        return true;
    }
}
