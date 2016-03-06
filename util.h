/*******************************************************************
 *
 * Filename: util.h
 * Date:     2013-09-21
 *
 * Description: This class contains util functions
 *
 *
 *******************************************************************/


#ifndef UTIL_H
#define UTIL_H

#include <QRgb>
#include <QPen>

class CUtil
{
public:



    enum ColorRgb_ET
    {
        BLACK,
        RED_1,
        ORANGE_1,
        YELLOW_1,
        GREEN_1,
        CYAN,
        BLUE,
        MAGENTA,
        BROWN,
        PINK,
        WHITE,
        INDIAN_RED,
        DARKORANGE,
        CADMIUMYELLOW,
        DARKGREEN,
        NAVY,
        DARKVIOLET,
        DARKGRAY,
        LIGHTYELLOW_1,
        LIGHTPINK_1,
        SGI_LIGHTBLUE,
        LIGHTBLUE,
        LIGHTCYAN_1,
        CRIMSON,
        LIGHTPINK,
        LIGHTPINK_2,
        LIGHTPINK_3,
        LIGHTPINK_4,
        PINK_1,
        PINK_2,
        PINK_3,
        PINK_4,
        PALEVIOLETRED,
        PALEVIOLETRED_1,
        PALEVIOLETRED_2,
        PALEVIOLETRED_3,
        PALEVIOLETRED_4,
        LAVENDERBLUSH_1,
        LAVENDERBLUSH_2,
        LAVENDERBLUSH_3,
        LAVENDERBLUSH_4,
        VIOLETRED_1,
        VIOLETRED_2,
        VIOLETRED_3,
        VIOLETRED_4,
        HOTPINK,
        HOTPINK_1,
        HOTPINK_2,
        HOTPINK_3,
        HOTPINK_4,
        RASPBERRY,
        DEEPPINK_1,
        DEEPPINK_2,
        DEEPPINK_3,
        DEEPPINK_4,
        MAROON_1,
        MAROON_2,
        MAROON_3,
        MAROON_4,
        MEDIUMVIOLETRED,
        VIOLETRED,
        ORCHID,
        ORCHID_1,
        ORCHID_2,
        ORCHID_3,
        ORCHID_4,
        THISTLE,
        THISTLE_1,
        THISTLE_2,
        THISTLE_3,
        THISTLE_4,
        PLUM_1,
        PLUM_2,
        PLUM_3,
        PLUM_4,
        PLUM,
        VIOLET,
        MAGENTA_2,
        MAGENTA_3,
        MAGENTA_4,
        PURPLE,
        MEDIUMORCHID,
        MEDIUMORCHID_1,
        MEDIUMORCHID_2,
        MEDIUMORCHID_3,
        MEDIUMORCHID_4,
        DARKORCHID,
        DARKORCHID_1,
        DARKORCHID_2,
        DARKORCHID_3,
        DARKORCHID_4,
        INDIGO,
        BLUEVIOLET,
        PURPLE_1,
        PURPLE_2,
        PURPLE_3,
        PURPLE_4,
        MEDIUMPURPLE,
        MEDIUMPURPLE_1,
        MEDIUMPURPLE_2,
        MEDIUMPURPLE_3,
        MEDIUMPURPLE_4,
        DARKSLATEBLUE,
        LIGHTSLATEBLUE,
        MEDIUMSLATEBLUE,
        SLATEBLUE,
        SLATEBLUE_1,
        SLATEBLUE_2,
        SLATEBLUE_3,
        SLATEBLUE_4,
        GHOSTWHITE,
        LAVENDER,
        BLUE_2,
        BLUE_3,
        BLUE_4,
        MIDNIGHTBLUE,
        COBALT,
        ROYALBLUE,
        ROYALBLUE_1,
        ROYALBLUE_2,
        ROYALBLUE_3,
        ROYALBLUE_4,
        CORNFLOWERBLUE,
        LIGHTSTEELBLUE,
        LIGHTSTEELBLUE_1,
        LIGHTSTEELBLUE_2,
        LIGHTSTEELBLUE_3,
        LIGHTSTEELBLUE_4,
        LIGHTSLATEGRAY,
        SLATEGRAY,
        SLATEGRAY_1,
        SLATEGRAY_2,
        SLATEGRAY_3,
        SLATEGRAY_4,
        DODGERBLUE_1,
        DODGERBLUE_2,
        DODGERBLUE_3,
        DODGERBLUE_4,
        ALICEBLUE,
        STEELBLUE,
        STEELBLUE_1,
        STEELBLUE_2,
        STEELBLUE_3,
        STEELBLUE_4,
        LIGHTSKYBLUE,
        LIGHTSKYBLUE_1,
        LIGHTSKYBLUE_2,
        LIGHTSKYBLUE_3,
        LIGHTSKYBLUE_4,
        SKYBLUE_1,
        SKYBLUE_2,
        SKYBLUE_3,
        SKYBLUE_4,
        SKYBLUE,
        DEEPSKYBLUE_1,
        DEEPSKYBLUE_2,
        DEEPSKYBLUE_3,
        DEEPSKYBLUE_4,
        PEACOCK,
        LIGHTBLUE_1,
        LIGHTBLUE_2,
        LIGHTBLUE_3,
        LIGHTBLUE_4,
        POWDERBLUE,
        CADETBLUE_1,
        CADETBLUE_2,
        CADETBLUE_3,
        CADETBLUE_4,
        TURQUOISE_1,
        TURQUOISE_2,
        TURQUOISE_3,
        TURQUOISE_4,
        CADETBLUE,
        DARKTURQUOISE,
        AZURE_1,
        AZURE_2,
        AZURE_3,
        AZURE_4,
        LIGHTCYAN_2,
        LIGHTCYAN_3,
        LIGHTCYAN_4,
        PALETURQUOISE_1,
        PALETURQUOISE_2,
        PALETURQUOISE_3,
        PALETURQUOISE_4,
        DARKSLATEGRAY,
        DARKSLATEGRAY_1,
        DARKSLATEGRAY_2,
        DARKSLATEGRAY_3,
        DARKSLATEGRAY_4,
        CYAN_2,
        CYAN_3,
        CYAN_4,
        TEAL,
        MEDIUMTURQUOISE,
        LIGHTSEAGREEN,
        MANGANESEBLUE,
        TURQUOISE,
        COLDGREY,
        TURQUOISEBLUE,
        AQUAMARINE_1,
        AQUAMARINE_2,
        AQUAMARINE_3,
        AQUAMARINE_4,
        MEDIUMSPRINGGREEN,
        MINTCREAM,
        SPRINGGREEN,
        SPRINGGREEN_1,
        SPRINGGREEN_2,
        SPRINGGREEN_3,
        MEDIUMSEAGREEN,
        SEAGREEN_1,
        SEAGREEN_2,
        SEAGREEN_3,
        SEAGREEN_4,
        EMERALDGREEN,
        MINT,
        COBALTGREEN,
        HONEYDEW_1,
        HONEYDEW_2,
        HONEYDEW_3,
        HONEYDEW_4,
        DARKSEAGREEN,
        DARKSEAGREEN_1,
        DARKSEAGREEN_2,
        DARKSEAGREEN_3,
        DARKSEAGREEN_4,
        PALEGREEN,
        PALEGREEN_1,
        PALEGREEN_2,
        PALEGREEN_3,
        PALEGREEN_4,
        LIMEGREEN,
        FORESTGREEN,
        GREEN_2,
        GREEN_3,
        GREEN_4,
        GREEN,
        SAPGREEN,
        LAWNGREEN,
        CHARTREUSE_1,
        CHARTREUSE_2,
        CHARTREUSE_3,
        CHARTREUSE_4,
        GREENYELLOW,
        DARKOLIVEGREEN_1,
        DARKOLIVEGREEN_2,
        DARKOLIVEGREEN_3,
        DARKOLIVEGREEN_4,
        DARKOLIVEGREEN,
        OLIVEDRAB,
        OLIVEDRAB_1,
        OLIVEDRAB_2,
        OLIVEDRAB_3,
        OLIVEDRAB_4,
        IVORY_1,
        IVORY_2,
        IVORY_3,
        IVORY_4,
        BEIGE,
        LIGHTYELLOW_2,
        LIGHTYELLOW_3,
        LIGHTYELLOW_4,
        LIGHTGOLDENRODYELLOW,
        YELLOW_2,
        YELLOW_3,
        YELLOW_4,
        WARMGREY,
        OLIVE,
        DARKKHAKI,
        KHAKI_1,
        KHAKI_2,
        KHAKI_3,
        KHAKI_4,
        KHAKI,
        PALEGOLDENROD,
        LEMONCHIFFON_1,
        LEMONCHIFFON_2,
        LEMONCHIFFON_3,
        LEMONCHIFFON_4,
        LIGHTGOLDENROD_1,
        LIGHTGOLDENROD_2,
        LIGHTGOLDENROD_3,
        LIGHTGOLDENROD_4,
        BANANA,
        GOLD_1,
        GOLD_2,
        GOLD_3,
        GOLD_4,
        CORNSILK_1,
        CORNSILK_2,
        CORNSILK_3,
        CORNSILK_4,
        GOLDENROD,
        GOLDENROD_1,
        GOLDENROD_2,
        GOLDENROD_3,
        GOLDENROD_4,
        DARKGOLDENROD,
        DARKGOLDENROD_1,
        DARKGOLDENROD_2,
        DARKGOLDENROD_3,
        DARKGOLDENROD_4,
        ORANGE_2,
        ORANGE_3,
        ORANGE_4,
        FLORALWHITE,
        OLDLACE,
        WHEAT,
        WHEAT_1,
        WHEAT_2,
        WHEAT_3,
        WHEAT_4,
        MOCCASIN,
        PAPAYAWHIP,
        BLANCHEDALMOND,
        NAVAJOWHITE_1,
        NAVAJOWHITE_2,
        NAVAJOWHITE_3,
        NAVAJOWHITE_4,
        EGGSHELL,
        TAN,
        BRICK,
        ANTIQUEWHITE,
        ANTIQUEWHITE_1,
        ANTIQUEWHITE_2,
        ANTIQUEWHITE_3,
        ANTIQUEWHITE_4,
        BURLYWOOD,
        BURLYWOOD_1,
        BURLYWOOD_2,
        BURLYWOOD_3,
        BURLYWOOD_4,
        BISQUE_1,
        BISQUE_2,
        BISQUE_3,
        BISQUE_4,
        MELON,
        CARROT,
        DARKORANGE_1,
        DARKORANGE_2,
        DARKORANGE_3,
        DARKORANGE_4,
        ORANGE,
        TAN_1,
        TAN_2,
        TAN_3,
        TAN_4,
        LINEN,
        PEACHPUFF_1,
        PEACHPUFF_2,
        PEACHPUFF_3,
        PEACHPUFF_4,
        SEASHELL_1,
        SEASHELL_2,
        SEASHELL_3,
        SEASHELL_4,
        SANDYBROWN,
        RAWSIENNA,
        CHOCOLATE,
        CHOCOLATE_1,
        CHOCOLATE_2,
        CHOCOLATE_3,
        CHOCOLATE_4,
        IVORYBLACK,
        FLESH,
        CADMIUMORANGE,
        BURNTSIENNA,
        SIENNA,
        SIENNA_1,
        SIENNA_2,
        SIENNA_3,
        SIENNA_4,
        LIGHTSALMON_1,
        LIGHTSALMON_2,
        LIGHTSALMON_3,
        LIGHTSALMON_4,
        CORAL,
        ORANGERED_1,
        ORANGERED_2,
        ORANGERED_3,
        ORANGERED_4,
        SEPIA,
        DARKSALMON,
        SALMON_1,
        SALMON_2,
        SALMON_3,
        SALMON_4,
        CORAL_1,
        CORAL_2,
        CORAL_3,
        CORAL_4,
        BURNTUMBER,
        TOMATO_1,
        TOMATO_2,
        TOMATO_3,
        TOMATO_4,
        SALMON,
        MISTYROSE_1,
        MISTYROSE_2,
        MISTYROSE_3,
        MISTYROSE_4,
        SNOW_1,
        SNOW_2,
        SNOW_3,
        SNOW_4,
        ROSYBROWN,
        ROSYBROWN_1,
        ROSYBROWN_2,
        ROSYBROWN_3,
        ROSYBROWN_4,
        LIGHTCORAL,
        INDIANRED,
        INDIANRED_1,
        INDIANRED_2,
        INDIANRED_4,
        INDIANRED_3,
        BROWN_1,
        BROWN_2,
        BROWN_3,
        BROWN_4,
        FIREBRICK,
        FIREBRICK_1,
        FIREBRICK_2,
        FIREBRICK_3,
        FIREBRICK_4,
        RED_2,
        RED_3,
        RED_4,
        MAROON,
        SGI_BEET,
        SGI_SLATEBLUE,
        SGI_TEAL,
        SGI_CHARTREUSE,
        SGI_OLIVEDRAB,
        SGI_BRIGHTGRAY,
        SGI_SALMON,
        SGI_DARKGRAY,
        SGI_GRAY,
        SGI_GRAY_1,
        SGI_GRAY_2,
        SGI_GRAY_3,
        SGI_GRAY_4,
        SGI_GRAY_5,
        SGI_LIGHTGRAY,
        SGI_GRAY_6,
        SGI_GRAY_7,
        SGI_GRAY_8,
        SGI_GRAY_9,
        WHITE_SMOKE,
        GAINSBORO,
        LIGHTGREY,
        SILVER,
        GRAY,
        DIMGRAY,
        GRAY_99,
        GRAY_98,
        GRAY_97,
        WHITE_SMOKE_1,
        GRAY_95,
        GRAY_94,
        GRAY_93,
        GRAY_92,
        GRAY_91,
        GRAY_90,
        GRAY_89,
        GRAY_88,
        GRAY_87,
        GRAY_86,
        GRAY_85,
        GRAY_84,
        GRAY_83,
        GRAY_82,
        GRAY_81,
        GRAY_80,
        GRAY_79,
        GRAY_78,
        GRAY_77,
        GRAY_76,
        GRAY_75,
        GRAY_74,
        GRAY_73,
        GRAY_72,
        GRAY_71,
        GRAY_70,
        GRAY_69,
        GRAY_68,
        GRAY_67,
        GRAY_66,
        GRAY_65,
        GRAY_64,
        GRAY_63,
        GRAY_62,
        GRAY_61,
        GRAY_60,
        GRAY_59,
        GRAY_58,
        GRAY_57,
        GRAY_56,
        GRAY_55,
        GRAY_54,
        GRAY_53,
        GRAY_52,
        GRAY_51,
        GRAY_50,
        GRAY_49,
        GRAY_48,
        GRAY_47,
        GRAY_46,
        GRAY_45,
        GRAY_44,
        GRAY_43,
        GRAY_42,
        DIMGRAY_1,
        GRAY_40,
        GRAY_39,
        GRAY_38,
        GRAY_37,
        GRAY_36,
        GRAY_35,
        GRAY_34,
        GRAY_33,
        GRAY_32,
        GRAY_31,
        GRAY_30,
        GRAY_29,
        GRAY_28,
        GRAY_27,
        GRAY_26,
        GRAY_25,
        GRAY_24,
        GRAY_23,
        GRAY_22,
        GRAY_21,
        GRAY_20,
        GRAY_19,
        GRAY_18,
        GRAY_17,
        GRAY_16,
        GRAY_15,
        GRAY_14,
        GRAY_13,
        GRAY_12,
        GRAY_11,
        GRAY_10,
        GRAY_9,
        GRAY_8,
        GRAY_7,
        GRAY_6,
        GRAY_5,
        GRAY_4,
        GRAY_3,
        GRAY_2,
        GRAY_1,
        MAX_NOF_RGB_COLORS
    };

#if 0
    enum ColorRgb_ET
    {
        INDIAN_RED,
        PINK,
        CRIMSON,
        THISTLE,
        PLUM_1,
        MAGENTA,
        PURPLE,
        BLUEVIOLET,
        LIGHTSLATEBLUE,
        MIDNIGHTBLUE,
        LIGHTSTEELBLUE,
        DODGERBLUE_1,
        ALICEBLUE,
        STEELBLUE,
        LIGHTSKYBLUE,
        SKYBLUE_1,
        SKYBLUE,
        DEEPSKYBLUE_1,
        PEACOCK,
        LIGHTBLUE,
        POWDERBLUE,
        CADETBLUE_1,
        TURQUOISE_1,
        CADETBLUE,
        DARKTURQUOISE,
        AZURE_1,
        LIGHTCYAN_1,
        PALETURQUOISE_1,
        DARKSLATEGRAY,
        CYAN,
        TEAL,
        MEDIUMTURQUOISE,
        LIGHTSEAGREEN,
        MANGANESEBLUE,
        TURQUOISE,
        COLDGREY,
        TURQUOISEBLUE,
        AQUAMARINE_1,
        MEDIUMSPRINGGREEN,
        MINTCREAM,
        SPRINGGREEN,
        MEDIUMSEAGREEN,
        SEAGREEN_1,
        EMERALDGREEN,
        MINT,
        COBALTGREEN,
        HONEYDEW_1,
        DARKSEAGREEN,
        PALEGREEN,
        LIMEGREEN,
        FORESTGREEN,
        GREEN_1,
        GREEN,
        DARKGREEN,
        SAPGREEN,
        LAWNGREEN,
        CHARTREUSE_1,
        GREENYELLOW,
        DARKOLIVEGREEN_1,
        IVORY_1,
        BEIGE,
        LIGHTYELLOW_1,
        LIGHTGOLDENRODYELLOW,
        YELLOW_1,
        WARMGREY,
        OLIVE,
        DARKKHAKI,
        KHAKI_1,
        PALEGOLDENROD,
        LEMONCHIFFON_1,
        LIGHTGOLDENROD_4,
        BANANA,
        GOLD_1,
        CORNSILK_1,
        GOLDENROD,
        DARKGOLDENROD,
        ORANGE_1,
        FLORALWHITE,
        OLDLACE,
        WHEAT,
        MOCCASIN,
        PAPAYAWHIP,
        BLANCHEDALMOND,
        NAVAJOWHITE_1,
        EGGSHELL,
        TAN,
        BRICK,
        CADMIUMYELLOW,
        ANTIQUEWHITE,
        BURLYWOOD,
        BISQUE_1,
        MELON,
        CARROT,
        DARKORANGE,
        ORANGE,
        TAN_1,
        LINEN,
        PEACHPUFF_1,
        SEASHELL_1,
        SANDYBROWN,
        RAWSIENNA,
        CHOCOLATE,
        IVORYBLACK,
        FLESH,
        CADMIUMORANGE,
        BURNTSIENNA,
        SIENNA,
        LIGHTSALMON_1,
        CORAL,
        ORANGERED_1,
        SEPIA,
        DARKSALMON,
        SALMON_1,
        CORAL_1,
        BURNTUMBER,
        TOMATO_1,
        SALMON,
        MISTYROSE_1,
        SNOW_1,
        ROSYBROWN,
        LIGHTCORAL,
        INDIANRED,
        BROWN,
        FIREBRICK,
        RED_1,
        MAROON,
        SGI_BEET,
        SGI_SLATEBLUE,
        SGI_LIGHTBLUE,
        SGI_TEAL,
        SGI_CHARTREUSE,
        SGI_OLIVEDRAB,
        SGI_BRIGHTGRAY,
        SGI_SALMON,
        SGI_DARKGRAY,
        SGI_GRAY,
        SGI_LIGHTGRAY,
        GAINSBORO,
        LIGHTGREY,
        SILVER,
        DARKGRAY,
        GRAY,
        DIMGRAY,
        BLACK,
        MAX_NOF_RGB_COLORS
    };
#endif

 #if 0
    enum ColorRgb_ET
    {
        INDIAN_RED,
        CRIMSON,
        LIGHTPINK,
        LIGHTPINK_1,
        LIGHTPINK_2,
        LIGHTPINK_3,
        LIGHTPINK_4,
        PINK,
        PINK_1,
        PINK_2,
        PINK_3,
        PINK_4,
        PALEVIOLETRED,
        PALEVIOLETRED_1,
        PALEVIOLETRED_2,
        PALEVIOLETRED_3,
        PALEVIOLETRED_4,
        LAVENDERBLUSH_1,
        LAVENDERBLUSH_2,
        LAVENDERBLUSH_3,
        LAVENDERBLUSH_4,
        VIOLETRED_1,
        VIOLETRED_2,
        VIOLETRED_3,
        VIOLETRED_4,
        HOTPINK,
        HOTPINK_1,
        HOTPINK_2,
        HOTPINK_3,
        HOTPINK_4,
        RASPBERRY,
        DEEPPINK_1,
        DEEPPINK_2,
        DEEPPINK_3,
        DEEPPINK_4,
        MAROON_1,
        MAROON_2,
        MAROON_3,
        MAROON_4,
        MEDIUMVIOLETRED,
        VIOLETRED,
        ORCHID,
        ORCHID_1,
        ORCHID_2,
        ORCHID_3,
        ORCHID_4,
        THISTLE,
        THISTLE_1,
        THISTLE_2,
        THISTLE_3,
        THISTLE_4,
        PLUM_1,
        PLUM_2,
        PLUM_3,
        PLUM_4,
        PLUM,
        VIOLET,
        MAGENTA,
        MAGENTA_2,
        MAGENTA_3,
        MAGENTA_4,
        PURPLE,
        MEDIUMORCHID,
        MEDIUMORCHID_1,
        MEDIUMORCHID_2,
        MEDIUMORCHID_3,
        MEDIUMORCHID_4,
        DARKVIOLET,
        DARKORCHID,
        DARKORCHID_1,
        DARKORCHID_2,
        DARKORCHID_3,
        DARKORCHID_4,
        INDIGO,
        BLUEVIOLET,
        PURPLE_1,
        PURPLE_2,
        PURPLE_3,
        PURPLE_4,
        MEDIUMPURPLE,
        MEDIUMPURPLE_1,
        MEDIUMPURPLE_2,
        MEDIUMPURPLE_3,
        MEDIUMPURPLE_4,
        DARKSLATEBLUE,
        LIGHTSLATEBLUE,
        MEDIUMSLATEBLUE,
        SLATEBLUE,
        SLATEBLUE_1,
        SLATEBLUE_2,
        SLATEBLUE_3,
        SLATEBLUE_4,
        GHOSTWHITE,
        LAVENDER,
        BLUE,
        BLUE_2,
        BLUE_3,
        BLUE_4,
        NAVY,
        MIDNIGHTBLUE,
        COBALT,
        ROYALBLUE,
        ROYALBLUE_1,
        ROYALBLUE_2,
        ROYALBLUE_3,
        ROYALBLUE_4,
        CORNFLOWERBLUE,
        LIGHTSTEELBLUE,
        LIGHTSTEELBLUE_1,
        LIGHTSTEELBLUE_2,
        LIGHTSTEELBLUE_3,
        LIGHTSTEELBLUE_4,
        LIGHTSLATEGRAY,
        SLATEGRAY,
        SLATEGRAY_1,
        SLATEGRAY_2,
        SLATEGRAY_3,
        SLATEGRAY_4,
        DODGERBLUE_1,
        DODGERBLUE_2,
        DODGERBLUE_3,
        DODGERBLUE_4,
        ALICEBLUE,
        STEELBLUE,
        STEELBLUE_1,
        STEELBLUE_2,
        STEELBLUE_3,
        STEELBLUE_4,
        LIGHTSKYBLUE,
        LIGHTSKYBLUE_1,
        LIGHTSKYBLUE_2,
        LIGHTSKYBLUE_3,
        LIGHTSKYBLUE_4,
        SKYBLUE_1,
        SKYBLUE_2,
        SKYBLUE_3,
        SKYBLUE_4,
        SKYBLUE,
        DEEPSKYBLUE_1,
        DEEPSKYBLUE_2,
        DEEPSKYBLUE_3,
        DEEPSKYBLUE_4,
        PEACOCK,
        LIGHTBLUE,
        LIGHTBLUE_1,
        LIGHTBLUE_2,
        LIGHTBLUE_3,
        LIGHTBLUE_4,
        POWDERBLUE,
        CADETBLUE_1,
        CADETBLUE_2,
        CADETBLUE_3,
        CADETBLUE_4,
        TURQUOISE_1,
        TURQUOISE_2,
        TURQUOISE_3,
        TURQUOISE_4,
        CADETBLUE,
        DARKTURQUOISE,
        AZURE_1,
        AZURE_2,
        AZURE_3,
        AZURE_4,
        LIGHTCYAN_1,
        LIGHTCYAN_2,
        LIGHTCYAN_3,
        LIGHTCYAN_4,
        PALETURQUOISE_1,
        PALETURQUOISE_2,
        PALETURQUOISE_3,
        PALETURQUOISE_4,
        DARKSLATEGRAY,
        DARKSLATEGRAY_1,
        DARKSLATEGRAY_2,
        DARKSLATEGRAY_3,
        DARKSLATEGRAY_4,
        CYAN,
        CYAN_2,
        CYAN_3,
        CYAN_4,
        TEAL,
        MEDIUMTURQUOISE,
        LIGHTSEAGREEN,
        MANGANESEBLUE,
        TURQUOISE,
        COLDGREY,
        TURQUOISEBLUE,
        AQUAMARINE_1,
        AQUAMARINE_2,
        AQUAMARINE_3,
        AQUAMARINE_4,
        MEDIUMSPRINGGREEN,
        MINTCREAM,
        SPRINGGREEN,
        SPRINGGREEN_1,
        SPRINGGREEN_2,
        SPRINGGREEN_3,
        MEDIUMSEAGREEN,
        SEAGREEN_1,
        SEAGREEN_2,
        SEAGREEN_3,
        SEAGREEN_4,
        EMERALDGREEN,
        MINT,
        COBALTGREEN,
        HONEYDEW_1,
        HONEYDEW_2,
        HONEYDEW_3,
        HONEYDEW_4,
        DARKSEAGREEN,
        DARKSEAGREEN_1,
        DARKSEAGREEN_2,
        DARKSEAGREEN_3,
        DARKSEAGREEN_4,
        PALEGREEN,
        PALEGREEN_1,
        PALEGREEN_2,
        PALEGREEN_3,
        PALEGREEN_4,
        LIMEGREEN,
        FORESTGREEN,
        GREEN_1,
        GREEN_2,
        GREEN_3,
        GREEN_4,
        GREEN,
        DARKGREEN,
        SAPGREEN,
        LAWNGREEN,
        CHARTREUSE_1,
        CHARTREUSE_2,
        CHARTREUSE_3,
        CHARTREUSE_4,
        GREENYELLOW,
        DARKOLIVEGREEN_1,
        DARKOLIVEGREEN_2,
        DARKOLIVEGREEN_3,
        DARKOLIVEGREEN_4,
        DARKOLIVEGREEN,
        OLIVEDRAB,
        OLIVEDRAB_1,
        OLIVEDRAB_2,
        OLIVEDRAB_3,
        OLIVEDRAB_4,
        IVORY_1,
        IVORY_2,
        IVORY_3,
        IVORY_4,
        BEIGE,
        LIGHTYELLOW_1,
        LIGHTYELLOW_2,
        LIGHTYELLOW_3,
        LIGHTYELLOW_4,
        LIGHTGOLDENRODYELLOW,
        YELLOW_1,
        YELLOW_2,
        YELLOW_3,
        YELLOW_4,
        WARMGREY,
        OLIVE,
        DARKKHAKI,
        KHAKI_1,
        KHAKI_2,
        KHAKI_3,
        KHAKI_4,
        KHAKI,
        PALEGOLDENROD,
        LEMONCHIFFON_1,
        LEMONCHIFFON_2,
        LEMONCHIFFON_3,
        LEMONCHIFFON_4,
        LIGHTGOLDENROD_1,
        LIGHTGOLDENROD_2,
        LIGHTGOLDENROD_3,
        LIGHTGOLDENROD_4,
        BANANA,
        GOLD_1,
        GOLD_2,
        GOLD_3,
        GOLD_4,
        CORNSILK_1,
        CORNSILK_2,
        CORNSILK_3,
        CORNSILK_4,
        GOLDENROD,
        GOLDENROD_1,
        GOLDENROD_2,
        GOLDENROD_3,
        GOLDENROD_4,
        DARKGOLDENROD,
        DARKGOLDENROD_1,
        DARKGOLDENROD_2,
        DARKGOLDENROD_3,
        DARKGOLDENROD_4,
        ORANGE_1,
        ORANGE_2,
        ORANGE_3,
        ORANGE_4,
        FLORALWHITE,
        OLDLACE,
        WHEAT,
        WHEAT_1,
        WHEAT_2,
        WHEAT_3,
        WHEAT_4,
        MOCCASIN,
        PAPAYAWHIP,
        BLANCHEDALMOND,
        NAVAJOWHITE_1,
        NAVAJOWHITE_2,
        NAVAJOWHITE_3,
        NAVAJOWHITE_4,
        EGGSHELL,
        TAN,
        BRICK,
        CADMIUMYELLOW,
        ANTIQUEWHITE,
        ANTIQUEWHITE_1,
        ANTIQUEWHITE_2,
        ANTIQUEWHITE_3,
        ANTIQUEWHITE_4,
        BURLYWOOD,
        BURLYWOOD_1,
        BURLYWOOD_2,
        BURLYWOOD_3,
        BURLYWOOD_4,
        BISQUE_1,
        BISQUE_2,
        BISQUE_3,
        BISQUE_4,
        MELON,
        CARROT,
        DARKORANGE,
        DARKORANGE_1,
        DARKORANGE_2,
        DARKORANGE_3,
        DARKORANGE_4,
        ORANGE,
        TAN_1,
        TAN_2,
        TAN_3,
        TAN_4,
        LINEN,
        PEACHPUFF_1,
        PEACHPUFF_2,
        PEACHPUFF_3,
        PEACHPUFF_4,
        SEASHELL_1,
        SEASHELL_2,
        SEASHELL_3,
        SEASHELL_4,
        SANDYBROWN,
        RAWSIENNA,
        CHOCOLATE,
        CHOCOLATE_1,
        CHOCOLATE_2,
        CHOCOLATE_3,
        CHOCOLATE_4,
        IVORYBLACK,
        FLESH,
        CADMIUMORANGE,
        BURNTSIENNA,
        SIENNA,
        SIENNA_1,
        SIENNA_2,
        SIENNA_3,
        SIENNA_4,
        LIGHTSALMON_1,
        LIGHTSALMON_2,
        LIGHTSALMON_3,
        LIGHTSALMON_4,
        CORAL,
        ORANGERED_1,
        ORANGERED_2,
        ORANGERED_3,
        ORANGERED_4,
        SEPIA,
        DARKSALMON,
        SALMON_1,
        SALMON_2,
        SALMON_3,
        SALMON_4,
        CORAL_1,
        CORAL_2,
        CORAL_3,
        CORAL_4,
        BURNTUMBER,
        TOMATO_1,
        TOMATO_2,
        TOMATO_3,
        TOMATO_4,
        SALMON,
        MISTYROSE_1,
        MISTYROSE_2,
        MISTYROSE_3,
        MISTYROSE_4,
        SNOW_1,
        SNOW_2,
        SNOW_3,
        SNOW_4,
        ROSYBROWN,
        ROSYBROWN_1,
        ROSYBROWN_2,
        ROSYBROWN_3,
        ROSYBROWN_4,
        LIGHTCORAL,
        INDIANRED,
        INDIANRED_1,
        INDIANRED_2,
        INDIANRED_4,
        INDIANRED_3,
        BROWN,
        BROWN_1,
        BROWN_2,
        BROWN_3,
        BROWN_4,
        FIREBRICK,
        FIREBRICK_1,
        FIREBRICK_2,
        FIREBRICK_3,
        FIREBRICK_4,
        RED_1,
        RED_2,
        RED_3,
        RED_4,
        MAROON,
        SGI_BEET,
        SGI_SLATEBLUE,
        SGI_LIGHTBLUE,
        SGI_TEAL,
        SGI_CHARTREUSE,
        SGI_OLIVEDRAB,
        SGI_BRIGHTGRAY,
        SGI_SALMON,
        SGI_DARKGRAY,
        SGI_GRAY,
        SGI_GRAY_1,
        SGI_GRAY_2,
        SGI_GRAY_3,
        SGI_GRAY_4,
        SGI_GRAY_5,
        SGI_LIGHTGRAY,
        SGI_GRAY_6,
        SGI_GRAY_7,
        SGI_GRAY_8,
        SGI_GRAY_9,
        WHITE,
        WHITE_SMOKE,
        GAINSBORO,
        LIGHTGREY,
        SILVER,
        DARKGRAY,
        GRAY,
        DIMGRAY,
        BLACK,
        GRAY_99,
        GRAY_98,
        GRAY_97,
        WHITE_SMOKE_1,
        GRAY_95,
        GRAY_94,
        GRAY_93,
        GRAY_92,
        GRAY_91,
        GRAY_90,
        GRAY_89,
        GRAY_88,
        GRAY_87,
        GRAY_86,
        GRAY_85,
        GRAY_84,
        GRAY_83,
        GRAY_82,
        GRAY_81,
        GRAY_80,
        GRAY_79,
        GRAY_78,
        GRAY_77,
        GRAY_76,
        GRAY_75,
        GRAY_74,
        GRAY_73,
        GRAY_72,
        GRAY_71,
        GRAY_70,
        GRAY_69,
        GRAY_68,
        GRAY_67,
        GRAY_66,
        GRAY_65,
        GRAY_64,
        GRAY_63,
        GRAY_62,
        GRAY_61,
        GRAY_60,
        GRAY_59,
        GRAY_58,
        GRAY_57,
        GRAY_56,
        GRAY_55,
        GRAY_54,
        GRAY_53,
        GRAY_52,
        GRAY_51,
        GRAY_50,
        GRAY_49,
        GRAY_48,
        GRAY_47,
        GRAY_46,
        GRAY_45,
        GRAY_44,
        GRAY_43,
        GRAY_42,
        DIMGRAY_1,
        GRAY_40,
        GRAY_39,
        GRAY_38,
        GRAY_37,
        GRAY_36,
        GRAY_35,
        GRAY_34,
        GRAY_33,
        GRAY_32,
        GRAY_31,
        GRAY_30,
        GRAY_29,
        GRAY_28,
        GRAY_27,
        GRAY_26,
        GRAY_25,
        GRAY_24,
        GRAY_23,
        GRAY_22,
        GRAY_21,
        GRAY_20,
        GRAY_19,
        GRAY_18,
        GRAY_17,
        GRAY_16,
        GRAY_15,
        GRAY_14,
        GRAY_13,
        GRAY_12,
        GRAY_11,
        GRAY_10,
        GRAY_9,
        GRAY_8,
        GRAY_7,
        GRAY_6,
        GRAY_5,
        GRAY_4,
        GRAY_3,
        GRAY_2,
        GRAY_1,
        MAX_NOF_RGB_COLORS
     };
#endif

    struct ColorRgb_ST
    {
        int r;
        int g;
        int b;
    };




    static const ColorRgb_ST m_colorRgbArr[CUtil::MAX_NOF_RGB_COLORS];

    CUtil();
    QColor getQColor(CUtil::ColorRgb_ET color);
    QPen setColor(CUtil::ColorRgb_ET color);
    bool calcMeanAndStdDeviation(double inputData[], int nofData, double &mean, double &stdDev);
    bool calcMean(double inputData[], int nofData, double &mean);
    bool number2Int(QString number, int &intNumber);
    bool number2double(QString number, double &doubleNumber);
    bool dateIsValid(QString date);
    void getCurrentDate(QString &date);
    bool splitDate(QString date, QString &year, QString &month, QString &day);
    bool addDays(QString inputDate, QString &outputDate, int day);
    bool addMonth(QString inputDate, QString &outputDate, int intMonth);
    bool addYear(QString inputDate, QString &outputDate, int intYear);
    bool nofDaysBeteenDates(QString date1, QString date2, int &nofdays);
    bool createMatrixMemSpace(int nofStocks, double ***matrix);
    bool createArrMemSpace(int nofStocks, double **arr);
    void deleteArrMemSpace(double **arr);
    void removeMatrixMemSpace(int nofStocks, double ***matrix);
    bool number2DoubleRemoveSpace(QString number, double &dbNumber);
    void htmlCodesToChar(QString inStr, QString &outStr);
    bool annualGrowthRate(double startValue, double endValue, double nofYears, double &growthRate);
    bool createStartEndDates(QString &startDate,
                             QString &endDate,
                             int nofMonthInBetween);





};





#endif // UTIL_H
