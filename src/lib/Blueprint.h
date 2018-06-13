#pragma once
#include "Step.h"
#include <fruit/fruit_forward_decls.h>

class Blueprint {
public:
    virtual ~Blueprint() {}

    virtual std::shared_ptr<Step> getFirstStep() const = 0;

    virtual std::shared_ptr<Step> getNextStep(const std::shared_ptr<Step>& current, Status stat) const = 0;
};

fruit::Component<Blueprint> getBlueprint();