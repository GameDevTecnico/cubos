local counter = 0

cubos.startupSystem("print startup"):call(function()
	print("Startup lua system called!")
end)

cubos.system("inc counter"):call(function()
	counter = counter + 1
	if counter == 1000000 then
		print("Counter reached 1000000!")
		counter = 0
	end
end)
