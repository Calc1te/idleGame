//
// Created by Calcite on 2025/9/9.
//

#ifndef SETTINGS_H
#define SETTINGS_H



class Settings {
public:
    Settings();
    ~Settings();
    void load_settings();
    void save_settings();
    void confirm(int idx);
};



#endif //SETTINGS_H
