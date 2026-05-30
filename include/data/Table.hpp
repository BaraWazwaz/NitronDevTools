/**
 * @file Table.hpp
 * @brief implements @ref Table class that holds data in a serializable wrapper
 * @copyright 2026 Bara Wazwaz. Released under the GNU License
 */
#pragma once

#include <iostream>   // std::ostream, std::istream
#include <string>     // std::string
#include <vector>     // std::vector
#include <deque>      // std::deque
#include <memory>     // std::unique_ptr
#include <sstream>    // std::stringstream
#include <typeindex>  // std::type_index
#include <stdexcept>  // std::invalid_argument

namespace nitron
{

/// @brief data that are serializable using an iostream
template <typename Type>
concept Streamable = requires(std::ostream& os, 
                              std::istream& is, 
                              Type vm,
                              Type const& vc)
{
    { os << vc };
    { is >> vm };
};

class IDataHolder;

using DataHolderPtr = std::unique_ptr<IDataHolder>;

class IDataHolder
{
public:
    virtual ~IDataHolder() = default;
    
    [[nodiscard]]
    virtual DataHolderPtr clone() const = 0;
    
    [[nodiscard]]
    virtual std::type_index getType() const = 0;

    /// @brief read the value as a string
    [[nodiscard]]
    virtual std::string getString() const = 0;
    
    /// @brief replace the value with the one retrievable from string
    virtual void setString(std::string const& str) = 0;
};

template <Streamable Type>
class DataHolder final : public IDataHolder
{
public:
    [[nodiscard]]
    DataHolderPtr clone() const override;
    
    [[nodiscard]]
    std::type_index getType() const override;
    
    /// @brief read the value as a string
    [[nodiscard]]
    std::string getString() const override;
    
    /// @brief replace the value with the one retrievable from string
    void setString(std::string const& string) override;
    
    /// @brief read the value as it is originally stored
    [[nodiscard]]
    Type const& getValue() const;

    /// @brief replace the value with another one of the same type it is originally stored in
    template <typename... Args>
    void setValue(Args&&... args);

    /// @brief constructs object of original type by forwarding
    template <typename... Args>
    explicit DataHolder(Args&&... args);

private:
    Type m_data;
};

class Record
{
public:
    /// @brief add a field at an arbitrary index
    template <Streamable Type, typename... Args>
    void insertField(size_t index,
                     Args&&... value);

    /// @brief add a field at the end
    template <Streamable Type, typename... Args>
    void emplaceBackField(Args&&... args);

    /// @brief add a field at the beginning
    template <Streamable Type, typename... Args>
    void emplaceFrontField(Args&&... args);

    /// @brief remove a field located at an arbitrary index
    void eraseField(size_t index);
    /// @brief remove a field located at the end
    void popBackField();
    /// @brief remove a field located at the beginning
    void popFrontField();

    /// @brief access data by field index as an @ref IDataHolder
    [[nodiscard]]
    IDataHolder& at(size_t index);

    /// @brief access data by field index as an @ref IDataHolder
    [[nodiscard]]
    IDataHolder const& at(size_t index) const;

    /// @brief access data at field index as a value of original type
    /// @tparam Type expected original type of the stored value
    /// @throws `std::invalid_argument` when the type expected does not match original type
    template <Streamable Type>
    [[nodiscard]]
    Type const& getValueAt(size_t index) const;
    
    /// @brief replace data at field index with a value of original type
    /// @tparam Type expected original type of the stored value
    /// @throws `std::invalid_argument` when the type expected does not match original type
    template <Streamable Type, typename... Args>
    void setValueAt(size_t index,
                    Args&&... args);
    
    /// @brief access data at field index as a string
    [[nodiscard]]
    std::string getStringAt(size_t index) const;

    /// @brief replace data at field index with the one retrievable from string
    void setStringAt(size_t index, std::string const& value);

    /// @brief number of fields
    [[nodiscard]] size_t degree() const;

    /// @brief checks whether two records have the same corresponding datatypes, 
    ///        and reports the mismatch if found
    ///
    /// @returns
    /// - @ref Record::degree() if completely compatible
    ///
    /// - @ref Record::SIZE_MISMATCH if records had different degrees
    ///
    /// - an index indicating the first field a mismatch occurs otherwise
    [[nodiscard]]
    size_t incompatability(Record const& record) const;

    /// constant value returned by @ref Record::incompatability when record degrees differ
    inline static const size_t SIZE_MISMATCH = ULLONG_MAX;

private:
    std::deque<std::unique_ptr<IDataHolder>> m_cells;
};

class Table
{
public:
    /// @brief add a field at an arbitrary index to all records
    /// @tparam Type datatype of values held by the added field
    /// @param header name of added field
    /// @param ...args arguments used to construct default value of added field
    template <Streamable Type, typename... Args>
    void insertField(size_t index, std::string const& header, Args&&... args);

    /// @brief add a field at the end to all records
    /// @tparam Type datatype of values held by the added field
    /// @param header name of added field
    /// @param ...args arguments used to construct default value of added field
    template <Streamable Type, typename... Args>
    void emplaceBackField(std::string const& header, Args&&... args);
    
    /// @brief add a field at the beginnning to all records
    /// @tparam Type datatype of values held by the added field
    /// @param header name of added field
    /// @param ...args arguments used to construct default value of added field
    template <Streamable Type, typename... Args>
    void emplaceFrontField(std::string const& header, Args&&... args);

    /// @brief remove a field at an arbitrary index from all records
    void eraseField(size_t index);
    /// @brief remove a field at the end from all records
    void popBackField();
    /// @brief remove a field at the beginning from all records
    void popFrontField();
    
    /// @brief add a record at an arbitrary index of table
    void insertRecord(size_t index, Record&& record);
    /// @brief add a record at the end of table
    void emplaceBackRecord(Record&& record);
    /// @brief add a record at the beginning of table
    void emplaceFrontRecord(Record&& record);

    /// @brief remove a record from an arbitrary index of table
    void eraseRecord(size_t index);
    /// @brief remove a record from the end of table
    void popBackRecord();
    /// @brief remove a record from the beginning of table
    void popFrontRecord();

    Record& getRecord(size_t index);
    Record const& getRecord(size_t index) const;

    /// @brief cardinality of table
    [[nodiscard]] size_t rowCount() const;
    /// @brief degree of table
    [[nodiscard]] size_t columnCount() const;
    
    [[nodiscard]] std::string const& getHeader(size_t index) const;
    void setHeader(size_t index, std::string const& header);

    /// @brief maximum number of characters displayed from the 
    ///        value a single record's field when streaming table
    inline static const size_t MAX_FIELD_WIDTH = 12;
    
private:
    /// @brief checks if added record is compatible with prototype
    /// @throws `std::invalid_argument` when records are incompatible
    void assertCompatibility(Record const& record) const;

    std::deque<std::string> m_headers;
    Record m_prototypes;
    std::deque<Record> m_records;
};

std::ostream& operator<<(std::ostream& os, const Table& table);

}

#include "Table.tpp"