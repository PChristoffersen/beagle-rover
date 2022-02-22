#ifndef _ROBOT_LED_COLORARRAY_H_
#define _ROBOT_LED_COLORARRAY_H_

#include <memory>
#include <cstdint>
#include <string_view>
#include <algorithm>
#include <vector>
#include <mutex>
#include <initializer_list>
#include <boost/exception/all.hpp>

#include <robotconfig.h>
#include <common/withmutex.h>

namespace Robot::LED {

    /**
     * @brief An std::array of colors
     * 
     * An std::array of Color with additional operators for applying brightness and color correction.
     * 
     * @tparam NCOLORS Number of colors in the array
     */
    template<std::size_t NCOLORS>
    class ColorArray : public std::array<Color, NCOLORS>, public WithMutex<std::recursive_mutex> {
        public:
            using array_type = std::array<Color, NCOLORS>;

            /**
             * @brief A named partition of a color array
             */
            class Segment {
                public:
                    using parent_type = ColorArray<NCOLORS>; 
                    using size_type = typename parent_type::size_type;
                    using value_type = typename parent_type::value_type;

                    constexpr Segment(const std::string_view &name, size_type offset, size_type size) :
                        m_name { name },
                        m_offset { offset },
                        m_size { size },
                        m_parent { nullptr }
                    {
                    }

                    const std::string_view &name() const { return m_name; }
                    size_type offset() const { return m_offset; }
                    size_type size() const { return m_size; }

                    void fill(const value_type &val)
                    {
                        std::fill_n(m_parent->begin()+m_offset, m_size, val);
                    }



                    value_type &operator[](size_type pos) { return (*m_parent)[m_offset+pos]; }
                    const value_type &operator[](size_type pos) const { return (*m_parent)[m_offset+pos]; }

                    bool operator==(const Segment &other) const 
                    {
                        return m_offset==other.m_offset && m_size==other.m_size && m_name==other.m_name;
                    }

                    const parent_type *parent() const { return m_parent; }

                protected:
                    friend parent_type;
                    void setParent(parent_type *parent) 
                    {
                        if (parent->size()<m_offset+m_size) {
                            BOOST_THROW_EXCEPTION(std::invalid_argument((boost::format("Segment (%u,%u) is outside parent range (%u)") % m_offset % m_size % parent->size()).str()));
                        }
                        m_parent = parent;
                    }
                private:
                    const std::string_view m_name;
                    const size_type m_offset;
                    const size_type m_size;
                    parent_type *m_parent;

                    friend std::ostream &operator<<(std::ostream &os, const Segment &self)
                    {
                        return os << "Robot::LED::Segment<" << self.m_name << ">";
                    }

            };

            class SegmentList : public std::vector<Segment> {
                public:
                    using std::vector<Segment>::vector;

                    Segment &find(const std::string &name) 
                    {
                        return const_cast<Segment&>(const_cast<const SegmentList*>(this)->find(name));
                    }
                    const Segment &find(const std::string &name) const
                    {
                        for (const auto &s : *this) {
                            if (s.name()==name) {
                                return s;
                            }
                        }
                        BOOST_THROW_EXCEPTION(std::out_of_range("Error finding segment "+name));
                    }
            };

            ColorArray()
            {
            }
            ColorArray(const ColorArray<NCOLORS> &other) :
                array_type(other)
            {
            }
            ColorArray(const ColorArray<NCOLORS> &&other) :
                array_type(other)
            {
            }
            ColorArray(const std::initializer_list<Segment> &segments) :
                m_segments { segments }
            {
                for (auto &seg : m_segments) {
                    seg.setParent(this);
                }
            }

            /**
             * @brief Adjust the brightness of all colors in the array
             * 
             * @param brightness Brightness 0.0 - 1.0
             */
            ColorArray<NCOLORS> &operator*=(const Color::brightness_type brightness)
            {
                std::for_each(this->begin(), this->end(), [brightness](auto &dst) { dst *= brightness; });
                return *this;
            }

            /**
             * @brief Apply color correction to all the colors in the array
             * 
             * @param correction Correction to apply
             */
            ColorArray<NCOLORS> &operator*=(const Color::Correction correction)
            {
                std::for_each(this->begin(), this->end(), [correction](auto &dst) { dst *= correction; });
                return *this;
            }
            /**
             * @brief Blend all the colors from another array into this
             * 
             * @param other Array to blend
             */
            ColorArray<NCOLORS> &operator<<(const ColorArray<NCOLORS> &other)
            {
                std::transform(this->begin(), this->end(), other.begin(), this->begin(), [](auto &dst, const auto &src){ return dst<<src; });
                return *this;
            }

            SegmentList &segments() { return m_segments; }
            const SegmentList &segments() const { return m_segments; }
            Segment &segment(const std::string &name) { return m_segments.find(name); }
            const Segment &segment(const std::string &name) const { return m_segments.find(name); }

        private:
            SegmentList m_segments;
    };


    /**
     * @brief An std::array of colors
     * 
     * @tparam NCOLORS 
     */
    template<std::size_t NCOLORS>
    class RawColorArray : public std::array<Color::raw_type, NCOLORS> {
        public:
            using std::array<Color::raw_type, NCOLORS>::array;

            /**
             * @brief Construct a new RawColorArray from a ColorArray
             * 
             * @param array Source ColorArray
             */
            RawColorArray(const ColorArray<NCOLORS> &array)
            {
                std::transform(this->begin(), this->end(), array.begin(), this->begin(), [](auto &dst, const auto &src){ return (dst = src.raw()); });
            }
    };


}

#endif
