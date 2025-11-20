import React, { useState } from "react";
import { View, Text, StyleSheet, TextInput, TouchableOpacity, Alert } from "react-native";
import { useAuth } from "../context/AuthContext";

type Props = {
  navigation: any;
};

export default function LogInScreen({ navigation }: Props) {
  const { setUser } = useAuth();
  const [email, setEmail] = useState("");
  const [password, setPassword] = useState("");

  const handleLogin = () => {
    if (!email.trim() || !password.trim()) {
      Alert.alert("Missing information", "Please enter your email and password.");
      return;
    }

    // Backend will replace this with real Supabase login
    setUser({
      name: "JoyLab Friend",
      email: email.trim(),
    });
  };

  return (
    <View style={styles.container}>
      <Text style={styles.title}>Log In</Text>

      <TextInput
        style={styles.input}
        placeholder="Email"
        autoCapitalize="none"
        keyboardType="email-address"
        value={email}
        onChangeText={setEmail}
      />

      <TextInput
        style={styles.input}
        placeholder="Password"
        secureTextEntry
        value={password}
        onChangeText={setPassword}
      />

      <TouchableOpacity style={styles.primaryButton} onPress={handleLogin}>
        <Text style={styles.primaryButtonText}>Log In</Text>
      </TouchableOpacity>

      <TouchableOpacity onPress={() => navigation.navigate("Signup")}>
        <Text style={styles.switchText}>
          Don’t have an account? <Text style={{ color: "#4A7FFB" }}>Sign up</Text>
        </Text>
      </TouchableOpacity>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: "#F9FAFF",
    paddingHorizontal: 24,
    justifyContent: "center",
  },
  title: {
    fontSize: 28,
    fontWeight: "700",
    marginBottom: 24,
    textAlign: "center",
  },
  input: {
    backgroundColor: "#fff",
    borderRadius: 14,
    paddingHorizontal: 14,
    paddingVertical: 10,
    borderWidth: 1,
    borderColor: "#D0D4FF",
    marginBottom: 16,
  },
  primaryButton: {
    backgroundColor: "#4A7FFB",
    borderRadius: 20,
    paddingVertical: 12,
    alignItems: "center",
    marginBottom: 16,
  },
  primaryButtonText: {
    color: "#fff",
    fontSize: 16,
    fontWeight: "600",
  },
  switchText: {
    textAlign: "center",
    fontSize: 14,
    color: "#555",
  },
});
