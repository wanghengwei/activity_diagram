#pragma once

class Message {
public:
    int getId() const {
        return m_id;
    }

    int m_id{};
};
