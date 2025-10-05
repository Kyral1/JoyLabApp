import React from "react";
import { View, Text, StyleSheet, Button } from "react-native";

export default function SettingsScreen() {
  console.log("⚙️ SettingsScreen loaded!");
  return (
    <View style={styles.center}>
      <Text>Settings!</Text>
    </View>
  );
}

const styles = StyleSheet.create({
  center: { flex: 1, justifyContent: "center", alignItems: "center" },
});
