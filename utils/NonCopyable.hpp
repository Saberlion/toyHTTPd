#ifndef RECIPES_NONCOPYABLE_HPP
#define RECIPES_NONCOPYABLE_HPP
namespace saberUtils
{
    class Noncopyable
    {
    public:
        Noncopyable()=default;

        Noncopyable(const Noncopyable&) = delete ;
        Noncopyable(Noncopyable&&) = delete;
        Noncopyable& operator =(const Noncopyable& ) = delete;
        Noncopyable& operator =(Noncopyable&& ) = delete;
    };

}


#endif //RECIPES_NONCOPYABLE_HPP
