// Store user info globally while the app is running
import React, { createContext, useContext, useState } from "react";

export type User = {
  name: string;
  email: string;
};

// Replace setUser with Supabase code in LogInScreen
type AuthContextType = {
  user: User | null;
  setUser: (user: User | null) => void;
};

// Replace setUser with Supabase code in LogInScreen
const AuthContext = createContext<AuthContextType>({
  user: null,
  setUser: () => {},
});

// Replace setUser with Supabase code in LogInScreen
export const AuthProvider: React.FC<{ children: React.ReactNode }> = ({ children }) => {
  const [user, setUser] = useState<User | null>(null);

  return (
    <AuthContext.Provider value={{ user, setUser }}>
      {children}
    </AuthContext.Provider>
  );
};

export const useAuth = () => useContext(AuthContext);
