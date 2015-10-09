#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "archive.h"

#include <QObject>

#include <tuple>

class Serializer : public QObject
{
    Q_OBJECT
public:

    // TODO akasi: move it from here
    enum class MessageIds
    {
        eGLClear,
        eGLClearColor
    };

    explicit Serializer(QObject *parent = 0);

    template<typename... Args>
    Archive serialize(Args &&...args)
    {
        Archive ar;

        serializeArguments<0, Args...>(ar, std::forward_as_tuple(args...));
        return std::move(ar);
    }
signals:

public slots:

private:
    template <std::size_t I,
              typename... Args>
    inline typename std::enable_if < (I < sizeof...(Args)), void>::type
       serializeArguments(Archive &ar, std::tuple<Args...> args)
    {
        ar << std::get<I>(args);
        serializeArguments<I + 1, Args...>(ar, std::move(args));
    }

    template <std::size_t I,
              typename... Args>
    inline typename std::enable_if<(I == sizeof...(Args)), void>::type
       serializeArguments(Archive &, std::tuple<Args...>)
    {}
};

#endif // SERIALIZER_H
