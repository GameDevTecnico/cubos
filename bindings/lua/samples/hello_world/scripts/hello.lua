print("Hello world!")
print(type(cubos))

cubos.info("hello world from cubos", 123, 2.3, nil, {}, function() print("function") end)
cubos.warn("warning")
cubos.error("error")
cubos.critical("critical")
