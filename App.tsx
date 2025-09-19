import React from "react";
import { NavigationContainer } from "@react-navigation/native";
import { createBottomTabNavigator } from "@react-navigation/bottom-tabs";
import { View, Text, StyleSheet } from "react-native";

function SettingsScreen() {
  return <View style={styles.center}><Text>Settings</Text></View>;
}
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
