#pragma once

#include "Process.h"

class Date: public Process
{
    public:
        Date(pid_t datePid, int32_t dateJobNumber)
        {
            setProcessID(datePid);
            setJobNumber(dateJobNumber);
            setCommand("Date");

            // TODO: Finish constructor
        }

        int32_t execute() override;
};