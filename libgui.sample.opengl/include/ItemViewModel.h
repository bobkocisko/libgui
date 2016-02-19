#pragma once
#include <libgui/ViewModelBase.h>

#include <string>

class ItemViewModel: public libgui::ViewModelBase
{
public:
    ItemViewModel(const std::string& name, const std::string& type, const std::string& size);

    const std::string& GetName() const;
    void SetName(const std::string& name);

    const std::string& GetType() const;
    void SetType(const std::string& type);

    const std::string& GetSize() const;
    void SetSize(const std::string& size);

private:
    std::string name_;
    std::string type_;
    std::string size_;
};


