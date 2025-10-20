import React from "react";
import { NavigationContainer } from "@react-navigation/native";
import { createBottomTabNavigator } from "@react-navigation/bottom-tabs";
import { View, Text, StyleSheet, Button, Image } from "react-native";

import HomeScreen from "./src/frontend/screens/HomeScreen";
import SettingsScreen from "./src/frontend/screens/SettingsScreen";
import StatisticsScreen from "./src/frontend/screens/StatisticsScreen";
import GameModesScreen from "./src/frontend/screens/GameModesScreen";
import TestScreen from "./src/frontend/screens/TestScreen";

const Tab = createBottomTabNavigator();


export default function App() {
  return (
    <NavigationContainer>
      <Tab.Navigator
        screenOptions={{
          headerShown: false,
          tabBarShowLabel: true,
        }}
      >
        <Tab.Screen
          name="Home"
          component={HomeScreen}
          options={{
            tabBarIcon: ({ color, size }) => (
              <Image
                source={require("./src/frontend/components/home.png")}
                style={{ width: size, height: size, tintColor: color }}
              />
            ),
          }}
        />

        <Tab.Screen
          name="Settings"
          component={SettingsScreen}
          options={{
            tabBarIcon: ({ color, size }) => (
              <Image
                source={require("./src/frontend/components/gear.png")}
                style={{ width: size, height: size, tintColor: color }}
              />
            ),
          }}
        />

        <Tab.Screen
          name="Game Modes"
          component={GameModesScreen}
          options={{
            tabBarIcon: ({ color, size }) => (
              <Image
                source={require("./src/frontend/components/game.png")}
                style={{ width: size, height: size, tintColor: color }}
              />
            ),
          }}
        />

        <Tab.Screen
          name="Statistics"
          component={StatisticsScreen}
          options={{
            tabBarIcon: ({ color, size }) => (
              <Image
                source={require("./src/frontend/components/data1.png")}
                style={{ width: size, height: size, tintColor: color }}
              />
            ),
          }}
        />

        <Tab.Screen
          name="Test Screen"
          component={TestScreen}
        />

      </Tab.Navigator>
    </NavigationContainer>
  );
}

const styles = StyleSheet.create({
  center: { flex: 1, justifyContent: "center", alignItems: "center" },
});
