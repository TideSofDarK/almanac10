--[[
	Slightly buffed minotaur
--]]

c_minotaur_warrior_data = c_minotaur_data:extend()
c_minotaur_warrior_data.name = "Minotaur Warrior"
c_minotaur_warrior_data.max_health = 150
c_minotaur_warrior_data.max_mana = 1
c_minotaur_warrior_data.sprite_sheet_folder = "minotaur"
c_minotaur_warrior_data.movement_capability = MOVCAP_GROUND

c_minotaur_warrior = c_minotaur:extend()

c_minotaur_warrior.update = nil

function c_minotaur_warrior:on_spawn()
	print("Spawned minotaur warrior with ID:", self:get_index())
	print("Health is:", self:get_health())
end