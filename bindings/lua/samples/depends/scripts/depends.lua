transform = cubos.depends("cubos::engine::transformPlugin")

for i, v in pairs(transform) do
    cubos.info(i)
end

cubos.info("Type of position:", transform.Position.__kind)
cubos.info("Type of childof:", transform.ChildOf.__kind)
