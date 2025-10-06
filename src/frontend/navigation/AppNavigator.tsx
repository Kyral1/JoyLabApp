import React from 'react';
import { createBottomTabNavigator } from '@react-navigation/bottom-tabs';
import GameModesScreen from '../screens/GameModesScreen';
import SettingsScreen from '../screens/SettingsScreen';
import StatisticsScreen from '../screens/StatisticsScreen';
import HomeScreen from '../screens/HomeScreen';
import TestScreen from '../screens/TestScreen';

const Tab = createBottomTabNavigator();

export default function AppNavigator() {
  return (
    <Tab.Navigator>
      <Tab.Screen name="Home" component={HomeScreen} />
      <Tab.Screen name="Game Modes" component={GameModesScreen} />
      <Tab.Screen name="Settings" component={SettingsScreen} />
      <Tab.Screen name="Statistics" component={StatisticsScreen} />
      <Tab.Screen name="Test Screen" component={TestScreen} />
    </Tab.Navigator>
  );
}
