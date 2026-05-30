#include <iomanip>
#include "data/Table.hpp"

namespace nitron
{

void Record::popBackField()
{
    m_cells.pop_back();
}

void Record::popFrontField()
{
    m_cells.pop_front();
}

void Record::eraseField(size_t index)
{
    m_cells.erase(m_cells.cbegin() + index);
}

IDataHolder& Record::at(size_t index)
{
    return *m_cells.at(index);
}

IDataHolder const& Record::at(size_t index) const
{
    return *m_cells.at(index);
}

std::string Record::getStringAt(size_t index) const
{
    IDataHolder const& cell = *m_cells.at(index);
    return cell.getString();
}

void Record::setStringAt(size_t index, std::string const& value)
{
    IDataHolder& cell = *m_cells.at(index);
    cell.setString(value);
}

size_t Record::degree() const
{
    return m_cells.size();
}

size_t Record::incompatability(Record const& record) const
{
    size_t size = degree();
    if (record.degree() != size)
    {
        return SIZE_MISMATCH;
    }
    for (size_t index = 0; index < size; ++index)
    {
        if (record.at(index).getType() != at(index).getType())
        {
            return index;
        }
    }
    return size;
}

void Table::popBackField()
{
    m_headers.pop_back();
    m_prototypes.popBackField();
    for (Record& record : m_records)
    {
        record.popBackField();
    }
}

void Table::popFrontField()
{
    m_headers.pop_front();
    m_prototypes.popFrontField();
    for (Record& record : m_records)
    {
        record.popFrontField();
    }
}

void Table::eraseField(size_t index)
{
    m_headers.erase(m_headers.cbegin() + index);
    m_prototypes.eraseField(index);
    for (Record& record : m_records)
    {
        record.eraseField(index);
    }
}

void Table::emplaceBackRecord(Record&& record)
{
    assertCompatibility(record);
    m_records.emplace_back(std::forward<Record>(record));
}

void Table::emplaceFrontRecord(Record&& record)
{
    assertCompatibility(record);
    m_records.emplace_front(std::forward<Record>(record));
}

void Table::insertRecord(size_t index, Record&& record)
{
    assertCompatibility(record);
    m_records.emplace(m_records.cbegin() + index, std::forward<Record>(record));
}

void Table::popBackRecord()
{
    m_records.pop_back();
}

void Table::popFrontRecord()
{
    m_records.pop_front();
}

void Table::eraseRecord(size_t index)
{
    m_records.erase(m_records.cbegin() + index);
}

Record& Table::getRecord(size_t index)
{
    return m_records.at(index);
}

Record const& Table::getRecord(size_t index) const
{
    return m_records.at(index);
}

size_t Table::rowCount() const
{
    return m_records.size();
}

size_t Table::columnCount() const
{
    return m_prototypes.degree();
}

std::string const& Table::getHeader(size_t index) const
{
    return m_headers.at(index);
}

void Table::setHeader(size_t index, std::string const& header)
{
    m_headers.at(index) = header;
}

void Table::assertCompatibility(Record const& record) const
{
    size_t mismatch = m_prototypes.incompatability(record);
    if (mismatch != m_prototypes.degree())
    {
        if (mismatch == Record::SIZE_MISMATCH)
        {
            throw std::invalid_argument("could not Table::assertCompatibility() a Record of a different degree.");
        }
        else
        {
            throw std::invalid_argument("could not Table::assertCompatibility() a Record of an incompatible corresponding value");
        }
    }
}

std::ostream& operator<<(std::ostream& os, const Table& table)
{
    size_t rows    = table.rowCount();
    size_t columns = table.columnCount();
    std::vector<std::string> data (rows * columns);
    std::vector<std::string> headers (columns);

    const size_t width = Table::MAX_FIELD_WIDTH;        
    static auto clip = [](std::string& cell) -> void
    {
        if (cell.size() > width)
        {
            cell.resize(width);
            for (size_t character = 1; character <= 3; ++character)
            {
                cell.at(width - character) = '.';
            }
        }
    };
    
    for (size_t column = 0; column < columns; ++column)
    {
        std::string& cell = headers[column];
        cell = table.getHeader(column);
        clip(cell);
    }

    for (size_t row = 0; row < rows; ++row)
    {
        Record const& record = table.getRecord(row);
        for (size_t column = 0; column < columns; ++column)
        {
            std::string& cell = data[row * columns + column];
            cell = record.getStringAt(column);
            clip(cell);
        }
    }

    os << std::string(columns * (width + 3) + 1, '-') << "\n";
    os << "|";
    for (std::string const& header : headers)
    {
        os << " " << std::setw(width) << header << " |";
    }
    os << "\n";
    os << std::string(columns * (width + 3) + 1, '-') << "\n";
    for (size_t row = 0; row < rows; ++row)
    {
        os << "|";
        for (size_t column = 0; column < columns; ++column)
        {
            os << " " << std::setw(width) << data[row * columns + column] << " |";
        }
        os << "\n";
    }
    os << std::string(columns * (width + 3) + 1, '-') << "\n";
    return os;
}

}