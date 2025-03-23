#pragma once
#include <std_include.hpp>
#include <QtWidgets/QWidget>

namespace updater
{
    std::string get_server_version();
    bool check_for_updates();
    void download_and_update();
    bool check_and_prompt_for_updates(QWidget* parent);

    class UpdateManager
    {
    public:
    };
}
