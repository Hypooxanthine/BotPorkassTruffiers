#pragma once

#include <unordered_map>

#include "DAO/DAO.h"

template <typename ID, typename DTO, typename Map = std::unordered_map<ID, DTO>>
class AbstractMapDAO : public IDAO<ID, DTO>
{
public:
    using Map_Type = Map;

public:

    /**
     * @brief Get map of elements.
     * 
     * @return const Map_Type& The map of elements.
     */
    inline const Map_Type& getDataMap() const { return m_Elements; }

protected:
    Map_Type m_Elements;
};