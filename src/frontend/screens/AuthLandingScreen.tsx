import React from "react";
import { View, Text, StyleSheet, TouchableOpacity } from "react-native";

/*type Props = {
  navigation: any; // keep it simple for now
};*/

export default function AuthLandingScreen({ navigation }: any) {
  return (
    <View style={styles.container}>
      <Text style={styles.welcomeText}>Welcome to</Text>
      <Text style={styles.logoText}>
        <Text style={{ color: "#4A7FFB" }}>Joy</Text>
        <Text style={{ color: "#E74C3C" }}>LAB</Text>
      </Text>

      <TouchableOpacity
        style={[styles.button, styles.primary]}
        onPress={() => navigation.navigate("Login")}
      >
        <Text style={styles.primaryText}>Log In</Text>
      </TouchableOpacity>

      <TouchableOpacity
        style={[styles.button, styles.outline]}
        onPress={() => navigation.navigate("SignUp")}
      >
        <Text style={styles.outlineText}>Sign Up</Text>
      </TouchableOpacity>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: "#F9FAFF",
    alignItems: "center",
    justifyContent: "center",
    paddingHorizontal: 24,
  },
  welcomeText: {
    fontSize: 22,
    fontWeight: "500",
    color: "#333",
    marginBottom: 4,
  },
  logoText: {
    fontSize: 48,
    fontWeight: "800",
    marginBottom: 40,
  },
  button: {
    width: "100%",
    borderRadius: 20,
    paddingVertical: 12,
    paddingHorizontal: 24,
    marginVertical: 8,
    alignItems: "center",
  },
  primary: {
    backgroundColor: "#4A7FFB",
  },
  primaryText: {
    color: "#fff",
    fontSize: 16,
    fontWeight: "600",
  },
  outline: {
    borderWidth: 1.5,
    borderColor: "#4A7FFB",
    backgroundColor: "#fff",
  },
  outlineText: {
    color: "#4A7FFB",
    fontSize: 16,
    fontWeight: "600",
  },
});
