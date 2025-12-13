import React from "react";
import { NavigationContainer, getFocusedRouteNameFromRoute } from "@react-navigation/native";
import { createBottomTabNavigator } from "@react-navigation/bottom-tabs";
import { createNativeStackNavigator } from "@react-navigation/native-stack";
import { StyleSheet, Image } from "react-native";

import HomeScreen from "./src/frontend/screens/HomeScreen";
import SettingsScreen from "./src/frontend/screens/SettingsScreen";
import GameModesStack from "./src/frontend/navigation/GameModesStack";
import StatisticsScreen from "./src/frontend/screens/StatisticsScreen";
import ProfileScreen from "./src/frontend/screens/ProfileScreen";

import AuthLandingScreen from "./src/frontend/screens/AuthLandingScreen";
import LoginScreen from "./src/frontend/screens/LogInScreen";
import SignUpScreen from "./src/frontend/screens/SignUpScreen";
import { AuthProvider, useAuth } from "./src/frontend/context/AuthContext";

const Tab = createBottomTabNavigator();
const Stack = createNativeStackNavigator();

function MainTabs() {
  return (
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
        name="Modes"
        component={GameModesStack}
        options={({ route }) => {
          const rn = getFocusedRouteNameFromRoute(route) ?? "GameModesHome";
          const hide = rn !== "GameModesHome";
          return {
            tabBarIcon: ({ color, size }) => (
              <Image
                source={require("./src/frontend/components/game.png")}
                style={{ width: size, height: size, tintColor: color }}
              />
            ),
            tabBarStyle: hide ? { display: "none" } : undefined,
            headerShown: false,
          };
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
        name="Profile"
        component={ProfileScreen}
        options={{
          tabBarIcon: ({ color, size }) => (
            <Image
              source={require("./src/frontend/components/user.png")}
              style={{ width: size, height: size, tintColor: color }}
            />
          ),
        }}
      />
    </Tab.Navigator>
  );
}

function RootNavigator() {
  const { user } = useAuth(); // comes from AuthContext

  return (
    <Stack.Navigator screenOptions={{ headerShown: false }}>
      {user ? (
        // If logged in, show your existing Tab navigator
        <Stack.Screen name="Main" component={MainTabs} />
      ) : (
        // If NOT logged in, show the auth flow screens
        <>
          <Stack.Screen name="AuthLanding" component={AuthLandingScreen} />
          <Stack.Screen name="Login" component={LoginScreen} />
          <Stack.Screen name="SignUp" component={SignUpScreen} />
        </>
      )}
    </Stack.Navigator>
  );
}

export default function App() {
  return (
    <AuthProvider>
      <NavigationContainer>
        <RootNavigator />
      </NavigationContainer>
    </AuthProvider>
  );
}

const styles = StyleSheet.create({
  center: { flex: 1, justifyContent: "center", alignItems: "center" },
});
