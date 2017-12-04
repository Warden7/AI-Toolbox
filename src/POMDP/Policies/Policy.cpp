#include <AIToolbox/POMDP/Policies/Policy.hpp>

#include <AIToolbox/POMDP/Utils.hpp>

namespace AIToolbox::POMDP {
    Policy::Policy(const size_t s, const size_t a, const size_t o) :
            Base(s, a), O(o), H(0), policy_(makeValueFunction(S)) {}

    Policy::Policy(const size_t s, const size_t a, const size_t o, const ValueFunction & v) :
            Base(s, a), O(o), H(v.size()-1), policy_(v)
    {
        if ( !v.size() ) throw std::invalid_argument("The ValueFunction supplied to POMDP::Policy is empty.");
    }

    size_t Policy::sampleAction(const Belief & b) const {
        // We use the latest horizon here.
        auto & vlist = policy_.back();

        auto bestMatch = findBestAtBelief(b, std::begin(vlist), std::end(vlist));

        return std::get<ACTION>(*bestMatch);
    }

    std::tuple<size_t, size_t> Policy::sampleAction(const Belief & b, const unsigned horizon) const {
        const auto & vlist = policy_[horizon];

        const auto begin     = std::begin(vlist);
        const auto bestMatch = findBestAtBelief(b, begin, std::end(vlist));

        const size_t action = std::get<ACTION>(*bestMatch);
        const size_t id     = std::distance(begin, bestMatch);

        return std::make_tuple(action, id);
    }

    std::tuple<size_t, size_t> Policy::sampleAction(const size_t id, const size_t o, const unsigned horizon) const {
        // Horizon + 1 means one step in the past.
        // Note that the zero entry is never supposed to be used, and it's just
        // a byproduct of the computing process.
        const auto & vlist = policy_[horizon+1];

        const size_t newId  = std::get<OBS>(vlist[id])[o];
        const size_t action = std::get<ACTION>(policy_[horizon][newId]);

        return std::make_tuple(action, newId);
    }

    double Policy::getActionProbability(const Belief & b, const size_t & a) const {
        // At the moment we know that only one action is possible..
        const size_t trueA = sampleAction(b);

        return ( a == trueA ? 1.0 : 0.0 );
    }

    double Policy::getActionProbability(const Belief & b, const size_t a, const unsigned horizon) const {
        // At the moment we know that only one action is possible..
        const size_t trueA = std::get<0>(sampleAction(b, horizon));

        return ( a == trueA ? 1.0 : 0.0 );
    }

    size_t Policy::getO() const {
        return O;
    }

    size_t Policy::getH() const {
        return H;
    }

    const ValueFunction & Policy::getValueFunction() const {
        return policy_;
    }
}
