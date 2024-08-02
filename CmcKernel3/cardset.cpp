#include "pch.h"
#include "cardset.h"
#include "itertools.h"

template<typename t>
inline static void unique_inplace(vector<t>& v) {
    sort(v.begin(), v.end());
    v.erase(unique(v.begin(), v.end()), v.end());
}

inline static uc8 unityPart(uc8 x) {
    uc8 res = 0;
    uc* ptr = (uc*)&x;
    uc* res_ptr = (uc*)&res;
    for (int i = 0; i < 8; i++) {
        if (ptr[i] == 1)res_ptr[i] = 1;
    }
    return res;
}

inline static uc8 unify(uc8 x) {
    uc8 res = 0;
    uc* ptr = (uc*)&x;
    uc* res_ptr = (uc*)&res;
    for (int i = 0; i < 8; i++) {
        if (ptr[i] != 0)res_ptr[i] = 1;
    }
    return res;
}

template<int n>
inline static char byteExtract(uc8 x) {
    return (char)(x >> (8 * n));
}

template<uc factor>
inline static bool hasFactor(uc8 x) {
    return
        (byteExtract<0>(x) % factor == 0) && (byteExtract<1>(x) % factor == 0) && (byteExtract<2>(x) % factor == 0) && (byteExtract<3>(x) % factor == 0) &&
        (byteExtract<4>(x) % factor == 0) && (byteExtract<5>(x) % factor == 0) && (byteExtract<6>(x) % factor == 0) && (byteExtract<7>(x) % factor == 0);
}

template<int n>
inline static bool bytegt(uc8 a, uc8 b) {
    return byteExtract<n>(a) > byteExtract<n>(b);
}

inline static bool gt(uc8 a, uc8 b) {
    return
        bytegt<0>(a, b) || bytegt<1>(a, b) || bytegt<2>(a, b) || bytegt<3>(a, b) ||
        bytegt<4>(a, b) || bytegt<5>(a, b) || bytegt<6>(a, b) || bytegt<7>(a, b);
}

uc count(uc8 x) {
    return 
          byteExtract<0>(x) + byteExtract<1>(x) + byteExtract<2>(x) + byteExtract<3>(x)
        + byteExtract<4>(x) + byteExtract<5>(x) + byteExtract<6>(x) + byteExtract<7>(x);
}


typedef struct CardSetSolution {
    CardSet set;
    uc8 compressed_formula, compressed_set;

    bool operator==(const CardSetSolution& o) const {
        return set == o.set && compressed_formula == o.compressed_formula && compressed_set == o.compressed_set;
    }
    CardSet decompress(vector<uc>& indices) {
        uc* ptr = (uc*)&compressed_set;
        for (int i = 0; i < indices.size(); i++) {
            set += CardSet(indices[i], ptr[i]);
        }
        return set;
    }
} CSS;

struct CSSHasher {
    std::size_t operator()(const CSS& k) const {
        std::size_t hash = 0;
        std::hash<unsigned __int64> hash_func;

        // Hash the __base set
        for (int i = 0; i < 64; ++i) {
            hash ^= std::hash<unsigned char>{}(k.set.data[i]) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        }

        // Hash the compressed_formula and compressed_set
        hash ^= hash_func(k.compressed_formula) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        hash ^= hash_func(k.compressed_set) + 0x9e3779b9 + (hash << 6) + (hash >> 2);

        return hash;
    }
};

inline static uc8 true_div(uc8 x, uc a) {
    uc8 res = 0;
    uc* p = (uc*)&x;
    uc* q = (uc*)&res;
    for (int i = 0; i < 8; i++) {
        q[i] = p[i] / a;
    }
    return res;
}

inline static void subsets(uc8 x, vector<uc8>& dst) {
    // avoid to return a vector, because it causes memory reallocations
    dst.clear();
    for (uc8 i0 = 0; i0 <= byteExtract<0>(x); i0++) {
        for (uc8 i1 = 0; i1 <= byteExtract<1>(x); i1++) {
            for (uc8 i2 = 0; i2 <= byteExtract<2>(x); i2++) {
                for (uc8 i3 = 0; i3 <= byteExtract<3>(x); i3++) {
                    for (uc8 i4 = 0; i4 <= byteExtract<4>(x); i4++) {
                        for (uc8 i5 = 0; i5 <= byteExtract<5>(x); i5++) {
                            for (uc8 i6 = 0; i6 <= byteExtract<6>(x); i6++) {
                                for (uc8 i7 = 0; i7 <= byteExtract<7>(x); i7++) {
                                    dst.push_back(i0 | i1 << 8 | i2 << 16 | i3 << 24 | i4 << 32 | i5 << 40 | i6 << 48 | i7 << 56);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

struct CanonicalSplitter {
    uc8 src;
    std::vector<uc8> current_subsets;
    int subset_idx;
    uc factor;

    CanonicalSplitter(const uc8& x) {
        src = x;
        subset_idx = 1;     // the zeroth subset is 0x00
        factor = 2;
        subsets(true_div(src, factor), current_subsets);
    }
    bool updateSubsets() {
        while (factor <= 4) {
            subsets(true_div(src, factor), current_subsets);
            factor++;
            subset_idx = 1;
            if (!current_subsets.empty()) {
                return true;
            }
        }
        return false;
    }
    bool goes() {
        return !(factor >= 4 && subset_idx >= current_subsets.size());
    }
    uc8 next() {
        if (subset_idx >= current_subsets.size()) {
            if (updateSubsets())return 0xffffffff;
        }
        return current_subsets[subset_idx++];
    }
};

inline static void unique_join(vector<CSS>& a, vector<CSS>& b) {
    a.reserve(a.size() + b.size());
    for (auto& x : b) {
        if (!in(x, a))a.push_back(x);
    }
}

inline static vector<CSS> direct_product(const vector<CSS>& a, const vector<CSS>& b) {
    vector<CSS> c; c.reserve(a.size() * b.size());
    for (auto& x : a) {
        for (auto& y : b) {
            CSS z = { 
                x.set + y.set, 
                x.compressed_formula + y.compressed_formula, 
                x.compressed_set +  y.compressed_set
            };
            c.push_back(z);     // the resultant vector may has duplicates
        }
    }
    return c;
}


std::unordered_map<CSS, vector<CSS>, CSSHasher> css_cache;

vector<CSS> possibleCardSets_inner(const CSS& question) {

    // Check if the result is in the cache
    auto it = css_cache.find(question);
    if (it != css_cache.end()) {
        return it->second;
    }

    // the end of recursion
    if (question.compressed_formula == 0) {
        css_cache[question] = { question };
        return { question };
    };

    // elements that have a single atom lead to only one branch
    uc8 unity_element_cards = unityPart(question.compressed_formula);
    if (unity_element_cards != 0) {
        auto result = possibleCardSets_inner(
            { 
                question.set,
                question.compressed_formula - unity_element_cards,
                question.compressed_set + unity_element_cards
            }
        );
        css_cache[question] = result;
        return result;
    }
    
    // formula that has a factor can lead to other branches
    vector<CSS> lst;
    if (hasFactor<2>(question.compressed_formula)) {
        auto alst = possibleCardSets_inner(
            { 
                question.set + atom(2 + (Element_num - 2)), 
                question.compressed_formula / 2,
                question.compressed_set
            }
        );
        unique_join(lst, alst);
    }
    if (hasFactor<3>(question.compressed_formula)) {
        auto alst = possibleCardSets_inner(
            {
                question.set + atom(3 + (Element_num - 2)),
                question.compressed_formula / 3,
                question.compressed_set
            }
        );
        unique_join(lst, alst);
    }
    if (hasFactor<4>(question.compressed_formula)) {
        auto alst = possibleCardSets_inner(
            {
                question.set + atom(4 + (Element_num - 2)),
                question.compressed_formula / 4,
                question.compressed_set
            }
        );
        unique_join(lst, alst);
    }

    // non-unique, non-factor part can be split
    CanonicalSplitter iter(question.compressed_formula);
    while (iter.goes()) {
        uc8 part = iter.next();
        if (part == 0xffffffff)break;
        uc8 residue = question.compressed_formula - part;
        vector<CSS> a1 = possibleCardSets_inner({question.set, part, question.compressed_set});  // who holds `question.set` does not matter
        vector<CSS> a2 = possibleCardSets_inner({zero(), residue, 0});
        vector<CSS> alst = direct_product(a1, a2);
        unique_join(lst, alst);
    }

    css_cache[question] = lst;
    return lst;
}

/*
    return: a set of sets of cards, including all possible sets to represent a formula (a matter).
*/
vector<CardSet> possibleCardSets(const Formula& f)
{
    // elements that have a single atom do not matter.
    CardSet unity_element_cards = f.unityPart();
    Formula residue = f - unity_element_cards;

    // for each element that has more than one atoms, use the dynamic programming (DP) to solve the variations
    vector<uc> indices;
    uc8 question = residue.compress(indices);

    // calculate
    vector<CSS> result = possibleCardSets_inner({unity_element_cards, question, 0});
    
    // parse the compressed set
    vector<CardSet> res; res.reserve(result.size());
    for (auto& r : result) {
        res.push_back(r.decompress(indices));
    }
    return res;
}
