#pragma once

#include <unordered_map>

#include "DAO/DAO.h"

template <typename ID, typename DTO, typename Map = std::unordered_map<ID, DTO>>
class AbstractMapDAO : public IDAO<ID, DTO>
{
public:
    using Map_Type = Map;
protected:
    Map_Type m_Elements;
};