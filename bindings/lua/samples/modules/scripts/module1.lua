print("Module 1 loaded")

function somefunc()
    print("Module 1 somefunc called")
end

local utils = require("utils.utils")
utils.someutils()
