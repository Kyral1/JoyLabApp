import React from "react";
import { NavigationContainer } from "@react-navigation/native";
import { createBottomTabNavigator } from "@react-navigation/bottom-tabs";
import { View, Text, StyleSheet, Button } from "react-native";

import HomeScreen from "./src/frontend/screens/HomeScreen";
import SettingsScreen from "./src/frontend/screens/SettingsScreen";

function GameModesScreen() {
  return <View style={styles.center}><Text>Game Modes</Text></View>;
}
function StatisticsScreen() {
  return <View style={styles.center}><Text>Statistics</Text></View>;
}

const Tab = createBottomTabNavigator();

export default function App() {
  return (
    <NavigationContainer>
      <Tab.Navigator>
        <Tab.Screen name="Home" component={HomeScreen} />
        <Tab.Screen name="Settings" component={SettingsScreen} />
        <Tab.Screen name="Game Modes" component={GameModesScreen} />
        <Tab.Screen name="Statistics" component={StatisticsScreen} />
      </Tab.Navigator>
    </NavigationContainer>
  );
}

const styles = StyleSheet.create({
  center: { flex: 1, justifyContent: "center", alignItems: "center" },
});
