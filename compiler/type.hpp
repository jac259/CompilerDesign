#ifndef TYPE_HPP
#define TYPE_HPP

struct Type { virtual ~Type() = default; };
struct Bool_Type : Type {};
struct Int_Type : Type {};

#endif
