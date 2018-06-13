--[[
	Black Dragon
--]]

c_black_dragon_data 					= c_base_data:extend()
c_black_dragon_data.name 				= "Black Dragon"
c_black_dragon_data.max_health 			= 200
c_black_dragon_data.max_mana 			= 50
c_black_dragon_data.attack_dice_count 	= 3
c_black_dragon_data.attack_dice 		= 6
c_black_dragon_data.attack_bonus 		= 5
c_black_dragon_data.movement_capability = MOVCAP_FLY
c_black_dragon_data.sprite_sheet_folder = "dragon"

c_black_dragon = c_base:extend()

function c_black_dragon:update(delta_time)

end

function c_black_dragon:on_spawn()
	print("Spawned black dragon with ID:", self:get_index())
	print(self:get_health())
end