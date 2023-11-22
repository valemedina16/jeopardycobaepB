#include <iostream>
#include <map>
#include <iomanip>
#include <vector>
#include <string>

#include "TextTable.h"

TextTable::TextTable( char horizontal, char vertical, char corner) :
        _horizontal( horizontal ),
        _vertical( vertical ),
        _corner( corner )
{
}

void TextTable::setAlignment( unsigned i, TextTable::Alignment alignment )
{
    _alignment[ i ] = alignment;
}

TextTable::Alignment TextTable::alignment( unsigned i ) const
{ 
    return _alignment[ i ]; 
}

char TextTable::vertical() const
{ 
    return _vertical; 
}

char TextTable::horizontal() const
{ 
    return _horizontal; 
}

void TextTable::add( std::string const & content )
{
    _current.push_back( content );
}

void TextTable::endOfRow()
{
    _rows.push_back( _current );
    _current.assign( 0, "" );
}

template <typename Iterator>
void TextTable::addRow( Iterator begin, Iterator end )
{
    for( auto i = begin; i != end; ++i ) {
        add( * i ); 
    }
    endOfRow();
}

template <typename Container>
void TextTable::addRow( Container const & container )
{
    addRow( container.begin(), container.end() );
}

std::vector< TextTable::Row > const & TextTable::rows() const
{
    return _rows;
}

void TextTable::setup() const
{
    determineWidths();
    setupAlignment();
}

std::string TextTable::ruler() const
{
    std::string result;
    result += _corner;
    for( auto width = _width.begin(); width != _width.end(); ++ width ) {
        result += repeat( * width, _horizontal );
        result += _corner;
    }

    return result;
}

int TextTable::width( unsigned i ) const
{ 
    return _width[ i ]; 
}

std::string TextTable::repeat( unsigned times, char c )
{
    std::string result;
    for( ; times > 0; -- times )
        result += c;

    return result;
}

unsigned TextTable::columns() const
{
    return _rows[ 0 ].size();
}

void TextTable::determineWidths() const
{
    _width.assign( columns(), 0 );
    for ( auto rowIterator = _rows.begin(); rowIterator != _rows.end(); ++ rowIterator ) {
        TextTable::Row const & row = * rowIterator;
        for ( unsigned i = 0; i < row.size(); ++i ) {
            _width[ i ] = _width[ i ] > row[ i ].size() ? _width[ i ] : row[ i ].size();
        }
    }
}

void TextTable::setupAlignment() const
{
    for ( unsigned i = 0; i < columns(); ++i ) {
        if ( _alignment.find( i ) == _alignment.end() ) {
            _alignment[ i ] = TextTable::Alignment::LEFT;
        }
    }
}

std::ostream & operator<<( std::ostream & stream, TextTable const & table )
{
    table.setup();
    stream << table.ruler() << "\n";
    for ( auto rowIterator = table.rows().begin(); rowIterator != table.rows().end(); ++ rowIterator ) {
        TextTable::TextTable::Row const & row = * rowIterator;
        stream << table.vertical();
        for ( unsigned i = 0; i < row.size(); ++i ) {
            auto alignment = table.alignment( i ) == TextTable::Alignment::LEFT ? std::left : std::right;
            stream << std::setw( table.width( i ) ) << alignment << row[ i ];
            stream << table.vertical();
        }
        stream << "\n";
        stream << table.ruler() << "\n";
    }

    return stream;
}

