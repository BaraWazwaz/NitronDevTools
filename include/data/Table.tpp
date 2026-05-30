#pragma once

#include "data/Table.hpp"

namespace nitron
{

template <Streamable Type>
std::unique_ptr<IDataHolder> DataHolder<Type>::clone() const
{
    return std::make_unique<DataHolder<Type>>(m_data);
}

template <Streamable Type>
std::type_index DataHolder<Type>::getType() const
{
    return typeid(Type);
}

template <Streamable Type>
std::string DataHolder<Type>::getString() const
{
    std::stringstream str;
    str << m_data;
    return str.str();
}

template <Streamable Type>
void DataHolder<Type>::setString(std::string const& string)
{
    std::stringstream str;
    str << string;
    str >> m_data;
    if (str.fail())
    {
        throw std::invalid_argument("type of string received from DataHolder<Type>::setString did not match the type Type.");
    }
}

template <Streamable Type>
Type const& DataHolder<Type>::getValue() const
{
    return m_data;
}

template <Streamable Type>
template <typename... Args>
void DataHolder<Type>::setValue(Args&&... args)
{
    m_data = Type(std::forward<Type>(args)...);
}

template <Streamable Type>
template <typename... Args>
DataHolder<Type>::DataHolder(Args&&... args)
    : m_data(std::forward<Args>(args)...)
{}

template <Streamable Type, typename... Args>
void Record::emplaceBackField(Args&&... args)
{
    m_cells.emplace_back(
        std::make_unique<DataHolder<Type>>(
            std::forward<Type>(args)...
        )
    );
}

template <Streamable Type, typename... Args>
void Record::emplaceFrontField(Args&&... args)
{
    m_cells.emplace_front(
        std::make_unique<DataHolder<Type>>(
            std::forward<Type>(args)...
        )
    );
}

template <Streamable Type, typename... Args>
void Record::insertField(size_t index, Args&&... args)
{
    m_cells.emplace(m_cells.cbegin() + index,
        std::make_unique<DataHolder<Type>>(
            std::forward<Type>(args)...
        )
    );
}

template <Streamable Type>
Type const& Record::getValueAt(size_t index) const
{
    IDataHolder const& cell = *m_cells.at(index);
    if (typeid(Type) != cell.getType())
    {
        throw std::invalid_argument("type expected of field from Record::getValueAt<Type>() did not match type of data inside DataHolder");
    }
    DataHolder<Type> const& cellTyped = static_cast<DataHolder<Type> const&>(cell);
    return cellTyped.getValue();
}

template <Streamable Type, typename... Args>
void Record::setValueAt(size_t index, Args&&... args)
{
    IDataHolder const& cell = *m_cells.at(index);
    if (typeid(Type) != cell.getType())
    {
        throw std::invalid_argument("type expected of field from Record::setValueAt<Type>() did not match type of data inside DataHolder");
    }
    DataHolder<Type> const& cellTyped = static_cast<DataHolder<Type> const&>(cell);
    cellTyped.setValue(std::forward<Type>(args)...);
}

template <Streamable Type, typename... Args>
void Table::emplaceBackField(std::string const& header, Args&&... args)
{
    m_headers.emplace_back(header);
    m_prototypes.emplaceBackField<Type>(std::forward<Args>(args)...);
    for (Record& record : m_records)
    {
        record.emplaceBackField<Type>(std::forward<Args>(args)...);
    }
}

template <Streamable Type, typename... Args>
void Table::emplaceFrontField(std::string const& header, Args&&... args)
{
    m_headers.emplace_front(header);
    m_prototypes.emplaceFrontField<Type>(std::forward<Args>(args)...);
    for (Record& record : m_records)
    {
        record.emplaceFrontField<Type>(std::forward<Args>(args)...);
    }
}

template <Streamable Type, typename... Args>
void Table::insertField(size_t index, std::string const& header, Args&&... args)
{
    m_headers.emplace(m_headers.cbegin() + index, header);
    m_prototypes.insertField<Type>(index, std::forward<Args>(args)...);
    for (Record& record : m_records)
    {
        record.insertField<Type>(index, std::forward<Args>(args)...);
    }
}

}